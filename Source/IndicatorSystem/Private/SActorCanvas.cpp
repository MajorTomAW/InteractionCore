// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.


#include "SActorCanvas.h"
#include "SActorCanvasArrowWidget.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/SLeafWidget.h"
#include "SlateOptMacros.h"
#include "SceneView.h"
#include "Engine/GameViewportClient.h"
#include "Layout/ArrangedChildren.h"

#include "IActorIndicatorWidget.h"
#include "IndicatorDescriptor.h"
#include "IndicatorManagerComponent.h"

class FSlateRect;

/** Roughly describes the direction of the arrow */
namespace EArrowDirection
{
	enum Type
	{
		Left,
		Top,
		Right,
		Bottom,
		MAX
	};
}

/** Angles for the direction of the arrow to display */
constexpr float ArrowRotations[EArrowDirection::MAX] =
{
	270.0f,
	0.0f,
	90.0f,
	180.0f
};

/** Offsets for the direction that the arrow can point */
const FVector2D ArrowOffsets[EArrowDirection::MAX] =
{
	FVector2D(-1.0f, 0.0f),
	FVector2D(0.0f, -1.0f),
	FVector2D(1.0f, 0.0f),
	FVector2D(0.0f, 1.0f)
};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SActorCanvas::Construct(
	const FArguments& InArgs, const FLocalPlayerContext& InContext, const FSlateBrush* InActorCanvasArrowBrush)
{
	LocalPlayerContext = InContext;
	ActorCanvasArrowBrush = InActorCanvasArrowBrush;

	IndicatorPool.SetWorld(LocalPlayerContext.GetWorld());

	SetCanTick(false);
	SetVisibility(EVisibility::SelfHitTestInvisible);

	// Create 10 arrows for starters
	for (int32 i = 0; i < 10; ++i)
	{
		TSharedRef<SActorCanvasArrowWidget> ArrowWidget = SNew(SActorCanvasArrowWidget, ActorCanvasArrowBrush);
		ArrowWidget->SetVisibility(EVisibility::Collapsed);

		ArrowChildren.AddSlot(MoveTemp
			(
				FArrowSlot::FSlotArguments(MakeUnique<FArrowSlot>())
				[
					ArrowWidget
				]
			));
	}

	UpdateActiveTimer();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


EActiveTimerReturnType SActorCanvas::UpdateCanvas(double InCurrentTime, float InDeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_UpdateCanvas);

	if (!OptionalPaintGeometry.IsSet())
	{
		return EActiveTimerReturnType::Continue;
	}

	// Grab the local player
	ULocalPlayer* LocalPlayer = LocalPlayerContext.GetLocalPlayer();
	UIndicatorManagerComponent* IndicatorComponent = IndicatorComponentPtr.Get();

	if (IndicatorComponent == nullptr)
	{
		IndicatorComponent = UIndicatorManagerComponent::FindIndicatorManagerComponent(LocalPlayerContext.GetPlayerController());
		if (IndicatorComponent)
		{
			// The world may have changed (due to travel)
			IndicatorPool.SetWorld(LocalPlayerContext.GetWorld());

			IndicatorComponentPtr = IndicatorComponent;
			IndicatorComponent->GetOnIndicatorAdded().AddSP(this, &SActorCanvas::OnIndicatorAdded);
			IndicatorComponent->GetOnIndicatorRemoved().AddSP(this, &SActorCanvas::OnIndicatorRemoved);

			for (UIndicatorDescriptor* Indicator : IndicatorComponent->GetIndicators())
			{
				OnIndicatorAdded(Indicator);
			}
		}
		else
		{
			//TODO HIDE EVERYTHING
			return EActiveTimerReturnType::Continue;
		}
	}

	// Make sure we have a local player.
	// If we don't, we can't project anything
	if (LocalPlayer)
	{
		const FGeometry PaintGeometry = OptionalPaintGeometry.GetValue();

		FSceneViewProjectionData ProjectionData;
		if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
		{
			SetShowAnyIndicators(true);
			bool bIndicatorsChanged = false;

			for (int32 ChildIdx = 0; ChildIdx < CanvasChildren.Num(); ++ChildIdx)
			{
				SActorCanvas::FSlot& CurChild = CanvasChildren[ChildIdx];
				UIndicatorDescriptor* Indicator = CurChild.Indicator;

				// if the slot content is invalid and we have permission to remove it
				if (Indicator->CanAutomaticallyRemove())
				{
					bIndicatorsChanged = true;

					RemoveIndicatorForEntry(Indicator);

					// Decrement the index to account for the removal
					--ChildIdx;
					continue;
				}

				// Update the indicator's visibility
				CurChild.SetIsIndicatorVisible(Indicator->GetIsVisible());

				if (!CurChild.GetIsIndicatorVisible())
				{
					bIndicatorsChanged |= CurChild.bIsDirty();
					CurChild.ClearDirtyFlag();
					continue;
				}

				// If the indicator changed clamp status between updates, alert the indicator and mark the indicators as changed
				if (CurChild.WasIndicatorClampedStatusChanged())
				{
					CurChild.ClearIndicatorClampedStatusChangedFlag();
					bIndicatorsChanged = true;
				}

				FVector ScreenPositionWithDepth;
				FIndicatorProjection Projection;
				const bool bSuccess = Projection.Project(*Indicator, ProjectionData, PaintGeometry.Size, OUT ScreenPositionWithDepth);

				if (!bSuccess)
				{
					CurChild.SetHasValidScreenPosition(false);
					CurChild.SetWasIndicatorClamped(false);

					bIndicatorsChanged |= CurChild.bIsDirty();
					CurChild.ClearDirtyFlag();
					continue;
				}

				CurChild.SetInFrontOfCamera(bSuccess);
				CurChild.SetHasValidScreenPosition(CurChild.GetInFrontOfCamera() || Indicator->GetClampToScreen());

				if (CurChild.HasValidScreenPosition())
				{
					// Only dirty the screen position if we can actually show this indicator
					CurChild.SetScreenPosition(FVector2D(ScreenPositionWithDepth));
					CurChild.SetDepth(ScreenPositionWithDepth.X);
				}

				// Update the priority of the indicator
				CurChild.SetPriority(Indicator->GetPriority());

				bIndicatorsChanged |= CurChild.bIsDirty();
				CurChild.ClearDirtyFlag();
			}

			if (bIndicatorsChanged)
			{
				Invalidate(EInvalidateWidgetReason::Paint);
			}
		}
		else
		{
			SetShowAnyIndicators(false);
		}
	}
	else
	{
		SetShowAnyIndicators(false);
	}

	if (AllIndicators.Num() == 0)
	{
		TickHandle.Reset();
		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

void SActorCanvas::SetShowAnyIndicators(bool bInShowAnyIndicators)
{
	if (bShowAnyIndicators != bInShowAnyIndicators)
	{
		bShowAnyIndicators = bInShowAnyIndicators;

		if (!bShowAnyIndicators)
		{
			for (int32 ChildIdx = 0; ChildIdx < AllChildren.Num(); ChildIdx++)
			{
				AllChildren.GetChildAt(ChildIdx)->SetVisibility(EVisibility::Collapsed);
			}
		}
	}
}

void SActorCanvas::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_OnArrangeChildren);

	NextArrowIndex = 0;

	// Make sure we have a local player
	// If we don't, we can't project anything
	if (bShowAnyIndicators)
	{
		const FVector2D ArrowWidgetSize = ActorCanvasArrowBrush->GetImageSize();
		const FIntPoint FixedPadding = FIntPoint(10.0f, 10.0f) + FIntPoint(ArrowWidgetSize.X, ArrowWidgetSize.Y);
		const FVector Center = FVector(AllottedGeometry.Size * 0.5f, 0.0f);

		// Sort the children
		TArray<const SActorCanvas::FSlot*> SortedSlots;
		for (int32 ChildIndex = 0; ChildIndex < CanvasChildren.Num(); ++ChildIndex)
		{
			SortedSlots.Add(&CanvasChildren[ChildIndex]);
		}

		SortedSlots.StableSort([](const SActorCanvas::FSlot& A, const SActorCanvas::FSlot& B)
		{
			return A.GetPriority() == B.GetPriority() ? A.GetDepth() > B.GetDepth() : A.GetPriority() < B.GetPriority();
		});

		// Go through all the sorted children
		for (int32 ChildIndex = 0; ChildIndex < SortedSlots.Num(); ++ChildIndex)
		{
			//grab a child
			const SActorCanvas::FSlot& CurChild = *SortedSlots[ChildIndex];
			const UIndicatorDescriptor* Indicator = CurChild.Indicator;

			// Skip this indicator if it's invalid or has an invalid world position
			if (!ArrangedChildren.Accepts(CurChild.GetWidget()->GetVisibility()))
			{
				CurChild.SetWasIndicatorClamped(false);
				continue;
			}

			FVector2D ScreenPosition = CurChild.GetScreenPosition();
			const bool bInFrontOfCamera = CurChild.GetInFrontOfCamera();

			// Don't bother if we can't project the position and the indicator doesn't want to be clamped
			const bool bShouldClamp = Indicator->GetClampToScreen();

			//get the offset and final size of the slot
			FVector2D SlotSize, SlotOffset, SlotPaddingMin, SlotPaddingMax;
			GetOffsetAndSize(Indicator, SlotSize, SlotOffset, SlotPaddingMin, SlotPaddingMax);

			bool bWasIndicatorClamped = false;

			// If we don't have to clamp this thing, we can skip a lot of work
			if (bShouldClamp)
			{
				//figure out if we clamped to any edge of the screen
				EArrowDirection::Type ClampDir = EArrowDirection::MAX;

				// Determine the size of inner screen rect to clamp within
				const FIntPoint RectMin = FIntPoint(SlotPaddingMin.X, SlotPaddingMin.Y) + FixedPadding;
				const FIntPoint RectMax = FIntPoint(AllottedGeometry.Size.X - SlotPaddingMax.X, AllottedGeometry.Size.Y - SlotPaddingMax.Y) - FixedPadding;
				const FIntRect ClampRect(RectMin, RectMax);

				// Make sure the screen position is within the clamp rect
				if (!ClampRect.Contains(FIntPoint(ScreenPosition.X, ScreenPosition.Y)))
				{
					const FPlane Planes[] =
					{
						FPlane(FVector(1.0f, 0.0f, 0.0f), ClampRect.Min.X),	// Left
						FPlane(FVector(0.0f, 1.0f, 0.0f), ClampRect.Min.Y),	// Top
						FPlane(FVector(-1.0f, 0.0f, 0.0f), -ClampRect.Max.X),	// Right
						FPlane(FVector(0.0f, -1.0f, 0.0f), -ClampRect.Max.Y)	// Bottom
					};

					for (int32 i = 0; i < EArrowDirection::MAX; ++i)
					{
						FVector NewPoint;
						if (FMath::SegmentPlaneIntersection(Center, FVector(ScreenPosition, 0.0f), Planes[i], NewPoint))
						{
							ClampDir = (EArrowDirection::Type)i;
							ScreenPosition = FVector2D(NewPoint);
						}
					}
				}
				else if (!bInFrontOfCamera)
				{
					const float ScreenXNorm = ScreenPosition.X / (RectMax.X - RectMin.X);
					const float ScreenYNorm = ScreenPosition.Y / (RectMax.Y - RectMin.Y);
					//we need to pin this thing to the side of the screen
					if (ScreenXNorm < ScreenYNorm)
					{
						if (ScreenXNorm < (-ScreenYNorm + 1.0f))
						{
							ClampDir = EArrowDirection::Left;
							ScreenPosition.X = ClampRect.Min.X;
						}
						else
						{
							ClampDir = EArrowDirection::Bottom;
							ScreenPosition.Y = ClampRect.Max.Y;
						}
					}
					else
					{
						if (ScreenXNorm < (-ScreenYNorm + 1.0f))
						{
							ClampDir = EArrowDirection::Top;
							ScreenPosition.Y = ClampRect.Min.Y;
						}
						else
						{
							ClampDir = EArrowDirection::Right;
							ScreenPosition.X = ClampRect.Max.X;
						}
					}
				}

				bWasIndicatorClamped = (ClampDir != EArrowDirection::MAX);

				// should we show an arrow
				if (Indicator->GetShowClampToScreenArrow() &&
					bWasIndicatorClamped &&
					ArrowChildren.IsValidIndex(NextArrowIndex))
				{
					const FVector2D ArrowOffsetDirection = ArrowOffsets[ClampDir];
					const float ArrowRotation = ArrowRotations[ClampDir];

					//grab an arrow widget
					TSharedRef<SActorCanvasArrowWidget> ArrowWidgetToUse = StaticCastSharedRef<SActorCanvasArrowWidget>(ArrowChildren.GetChildAt(NextArrowIndex));
					NextArrowIndex++;

					//set the rotation of the arrow
					ArrowWidgetToUse->SetRotation(ArrowRotation);

					//figure out the magnitude of the offset
					const FVector2D OffsetMagnitude = (SlotSize + ArrowWidgetSize) * 0.5f;

					//used to center the arrow on the position
					const FVector2D ArrowCenteringOffset = -(ArrowWidgetSize * 0.5f);

					FVector2D ArrowAlignmentOffset = FVector2D::ZeroVector;
					switch (Indicator->VAlignment)
					{
					case VAlign_Top:
						ArrowAlignmentOffset = SlotSize * FVector2D(0.0f, 0.5f);
						break;
					case VAlign_Bottom:
						ArrowAlignmentOffset = SlotSize * FVector2D(0.0f, -0.5f);
						break;
					}

					//figure out the offset for the arrow
					const FVector2D WidgetOffset = (OffsetMagnitude * ArrowOffsetDirection);

					const FVector2D FinalOffset = (WidgetOffset + ArrowAlignmentOffset + ArrowCenteringOffset);

					//get the final position
					const FVector2D FinalPosition = (ScreenPosition + FinalOffset);

					ArrowWidgetToUse->SetVisibility(EVisibility::HitTestInvisible);

					// Inject the arrow on top of the indicator
					ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
						ArrowWidgetToUse,			// The child widget being arranged
						FinalPosition,				// Child's local position (i.e. position within parent)
						ArrowWidgetSize,			// Child's size
						1.f							// Child's scale
					));
				}
			}

			CurChild.SetWasIndicatorClamped(bWasIndicatorClamped);

			// Add the information about this child to the output list (ArrangedChildren)
			ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
				CurChild.GetWidget(),
				ScreenPosition + SlotOffset,
				SlotSize,
				1.f
			));
		}
	}

	if (NextArrowIndex < ArrowIndexLastUpdate)
	{
		for (int32 ArrowRemovedIndex = NextArrowIndex; ArrowRemovedIndex < ArrowIndexLastUpdate; ArrowRemovedIndex++)
		{
			ArrowChildren.GetChildAt(ArrowRemovedIndex)->SetVisibility(EVisibility::Collapsed);
		}
	}

	ArrowIndexLastUpdate = NextArrowIndex;
}

void SActorCanvas::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(AllIndicators);
}

int32 SActorCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_OnPaint);

	OptionalPaintGeometry = AllottedGeometry;

	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	ArrangeChildren(AllottedGeometry, ArrangedChildren);

	int32 MaxLayerId = LayerId;

	const FPaintArgs NewArgs = Args.WithNewParent(this);
	const bool bShouldBeEnabled = ShouldBeEnabled(bParentEnabled);

	for (const FArrangedWidget& CurWidget : ArrangedChildren.GetInternalArray())
	{
		if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
		{
			SWidget* MutableWidget = const_cast<SWidget*>(&CurWidget.Widget.Get());

			const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect, OutDrawElements, bDrawElementsInOrder ? MaxLayerId : LayerId, InWidgetStyle, bShouldBeEnabled);
			MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
		}
		else
		{
			//SlateGI - RemoveContent
		}
	}

	return MaxLayerId;
}

void SActorCanvas::OnIndicatorAdded(UIndicatorDescriptor* Indicator)
{
	AllIndicators.Add(Indicator);
	InactiveIndicators.Add(Indicator);

	AddIndicatorForEntry(Indicator);
}

void SActorCanvas::OnIndicatorRemoved(UIndicatorDescriptor* Indicator)
{
	RemoveIndicatorForEntry(Indicator);

	AllIndicators.Remove(Indicator);
	InactiveIndicators.Remove(Indicator);
}

void SActorCanvas::AddIndicatorForEntry(UIndicatorDescriptor* Indicator)
{
	// Async load the indicator, and pool the results so that it's easy to use and reuse the widgets.
	TSoftClassPtr<UUserWidget> IndicatorClass = Indicator->GetIndicatorClass();
	if (!IndicatorClass.IsNull())
	{
		TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr(Indicator);
		AsyncLoad(IndicatorClass, [this, IndicatorPtr, IndicatorClass]() {
			if (UIndicatorDescriptor* Indicator = IndicatorPtr.Get())
			{
				// While async loading this indicator widget we could have removed it.
				if (!AllIndicators.Contains(Indicator))
				{
					return;
				}

				// Create the widget from the pool.
				if (UUserWidget* IndicatorWidget = IndicatorPool.GetOrCreateInstance(TSubclassOf<UUserWidget>(IndicatorClass.Get())))
				{
					if (IndicatorWidget->GetClass()->ImplementsInterface(UActorIndicatorWidget::StaticClass()))
					{
						IActorIndicatorWidget::Execute_BindIndicator(IndicatorWidget, Indicator);
					}

					Indicator->IndicatorWidget = IndicatorWidget;

					InactiveIndicators.Remove(Indicator);

					AddActorSlot(Indicator)
					[
						SAssignNew(Indicator->CanvasHost, SBox)
						[
							IndicatorWidget->TakeWidget()
						]
					];
				}
			}
		});
		StartAsyncLoading();
	}
}

void SActorCanvas::RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator)
{
	if (UUserWidget* IndicatorWidget = Indicator->IndicatorWidget.Get())
	{
		if (IndicatorWidget->GetClass()->ImplementsInterface(UActorIndicatorWidget::StaticClass()))
		{
			IActorIndicatorWidget::Execute_UnbindIndicator(IndicatorWidget, Indicator);
		}

		Indicator->IndicatorWidget = nullptr;
		
		IndicatorPool.Release(IndicatorWidget);
	}

	TSharedPtr<SWidget> CanvasHost = Indicator->CanvasHost.Pin();
	if (CanvasHost.IsValid())
	{
		RemoveActorSlot(CanvasHost.ToSharedRef());
		Indicator->CanvasHost.Reset();
	}
}

SActorCanvas::FScopedWidgetSlotArguments SActorCanvas::AddActorSlot(UIndicatorDescriptor* Indicator)
{
	TWeakPtr<SActorCanvas> WeakCanvas = SharedThis(this);
	return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(Indicator), this->CanvasChildren, INDEX_NONE
		, [WeakCanvas](const FSlot*, int32)
		{
			if (TSharedPtr<SActorCanvas> Canvas = WeakCanvas.Pin())
			{
				Canvas->UpdateActiveTimer();
			}
		}};
}

int32 SActorCanvas::RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget)
{
	for (int32 SlotIdx = 0; SlotIdx < CanvasChildren.Num(); ++SlotIdx)
	{
		if ( SlotWidget == CanvasChildren[SlotIdx].GetWidget() )
		{
			CanvasChildren.RemoveAt(SlotIdx);

			UpdateActiveTimer();

			return SlotIdx;
		}
	}

	return -1;
}


void SActorCanvas::GetOffsetAndSize(const UIndicatorDescriptor* Indicator, FVector2D& OutSize, FVector2D& OutOffset,
                                    FVector2D& OutPaddingMin, FVector2D& OutPaddingMax) const
{
	//This might get used one day
	FVector2D AllottedSize = FVector2D::ZeroVector;

	//grab the desired size of the child widget
	TSharedPtr<SWidget> CanvasHost = Indicator->CanvasHost.Pin();
	if (CanvasHost.IsValid())
	{
		OutSize = CanvasHost->GetDesiredSize();
	}

	//handle horizontal alignment
	switch(Indicator->GetHAlign())
	{
	case HAlign_Right: // same as Align_Top
		OutOffset.X = 0.0f;
		OutPaddingMin.X = 0.0f;
		OutPaddingMax.X = OutSize.X;
		break;
		
	case HAlign_Center:
		OutOffset.X = (AllottedSize.X - OutSize.X) / 2.0f;
		OutPaddingMin.X = OutSize.X / 2.0f;
		OutPaddingMax.X = OutPaddingMin.X;
		break;
		
	case HAlign_Left: // same as Align_Bottom
		OutOffset.X = AllottedSize.X - OutSize.X;
		OutPaddingMin.X = OutSize.X;
		OutPaddingMax.X = 0.0f;
		break;
	}

	//Now, handle vertical alignment
	switch(Indicator->GetVAlign())
	{
	case VAlign_Top:
		OutOffset.Y = 0.0f;
		OutPaddingMin.Y = 0.0f;
		OutPaddingMax.Y = OutSize.Y;
		break;
		
	case VAlign_Center:
		OutOffset.Y = (AllottedSize.Y - OutSize.Y) / 2.0f;
		OutPaddingMin.Y = OutSize.Y / 2.0f;
		OutPaddingMax.Y = OutPaddingMin.Y;
		break;
		
	case VAlign_Bottom:
		OutOffset.Y = AllottedSize.Y - OutSize.Y;
		OutPaddingMin.Y = OutSize.Y;
		OutPaddingMax.Y = 0.0f;
		break;
	}
}

void SActorCanvas::UpdateActiveTimer()
{
	const bool NeedsTicks = AllIndicators.Num() > 0 || !IndicatorComponentPtr.IsValid();

	if (NeedsTicks && !TickHandle.IsValid())
	{
		TickHandle = RegisterActiveTimer(0, FWidgetActiveTimerDelegate::CreateSP(this, &SActorCanvas::UpdateCanvas));
	}
}

