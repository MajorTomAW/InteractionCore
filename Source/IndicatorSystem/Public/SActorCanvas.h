// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AsyncMixin.h"
#include "Widgets/SPanel.h"
#include "Blueprint/UserWidgetPool.h"

class UIndicatorManagerComponent;
class UIndicatorDescriptor;
class FActiveTimerHandle;
class FArrangedChildren;
class FChildren;
class FPaintArgs;
class FReferenceCollector;
class FSlateRect;
class FSlateWindowElementList;
class FWidgetStyle;
struct FSlateBrush;

class SActorCanvas : public SPanel, public FAsyncMixin, public FGCObject
{
public:
	SActorCanvas()
		: CanvasChildren(this)
		, ArrowChildren(this)
		, AllChildren(this)
	{
		AllChildren.AddChildren(CanvasChildren);
		AllChildren.AddChildren(ArrowChildren);
	}
	
	/** ActorCanvas-specific slot class */
	class FSlot : public TSlotBase<FSlot>
	{
		friend class SActorCanvas;
	public:
		FSlot(UIndicatorDescriptor* InIndicator)
			: TSlotBase()
			, Indicator(InIndicator)
			, ScreenPosition(FVector2D::ZeroVector)
			, Depth(0)
			, Priority(0.f)
			, bIsIndicatorVisible(true)
			, bInFrontOfCamera(true)
			, bHasValidScreenPosition(false)
			, bDirty(true)
			, bWasIndicatorClamped(false)
			, bWasIndicatorClampedStatusChanged(false)
		{
		}

		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
		SLATE_SLOT_END_ARGS()
		using TSlotBase<FSlot>::Construct;

	public:
		/** Get the visibility of the indicator */
		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }

		/** Set the visibility of the indicator */
		void SetIsIndicatorVisible(bool bVisible)
		{
			if (bIsIndicatorVisible != bVisible)
			{
				bIsIndicatorVisible = bVisible;
				bDirty = true;
			}

			RefreshVisibility();
		}

		/** Get the screen position of the indicator */
		FVector2D GetScreenPosition() const { return ScreenPosition; }

		/** Set the screen position of the indicator */
		void SetScreenPosition(FVector2D InScreenPosition)
		{
			if (ScreenPosition != InScreenPosition)
			{
				ScreenPosition = InScreenPosition;
				bDirty = true;
			}
		}

		/** Get the depth of the indicator */
		double GetDepth() const { return Depth; }

		/** Set the depth of the indicator */
		void SetDepth(double InDepth)
		{
			if (Depth != InDepth)
			{
				Depth = InDepth;
				bDirty = true;
			}
		}

		/** Get the priority of the indicator */
		int32 GetPriority() const { return Priority; }

		/** Set the priority of the indicator */
		void SetPriority(int32 InPriority)
		{
			if (Priority != InPriority)
			{
				Priority = InPriority;
				bDirty = true;
			}
		}

		/** Get whether the indicator is in front of the camera */
		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }

		/** Set whether the indicator is in front of the camera */
		void SetInFrontOfCamera(bool bInFront)
		{
			if (bInFrontOfCamera != bInFront)
			{
				bInFrontOfCamera = bInFront;
				bDirty = true;
			}

			RefreshVisibility();
		}

		/** Get whether the indicator has a valid screen position */
		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }

		/** Set whether the indicator has a valid screen position */
		void SetHasValidScreenPosition(bool bValidScreenPosition)
		{
			if (bHasValidScreenPosition != bValidScreenPosition)
			{
				bHasValidScreenPosition = bValidScreenPosition;
				bDirty = true;
			}

			RefreshVisibility();
		}

		/** Get whether the indicator is dirty and needs to be redrawn next frame */
		bool bIsDirty() const { return bDirty; }

		/** Clear the dirty flag */
		void ClearDirtyFlag()
		{
			bDirty = false;
		}

		/** Get whether the indicator was visually screen clamped last frame or not */
		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }

		/** Set whether the indicator was visually screen clamped last frame or not */
		void SetWasIndicatorClamped(bool bWasClamped) const
		{
			if (bWasClamped != bWasIndicatorClamped)
			{
				bWasIndicatorClamped = bWasClamped;
				bWasIndicatorClampedStatusChanged = true;
			}
		}

		/** Get whether the indicator was visually screen clamped status changed */
		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }

		/** Clear the indicator clamped status changed flag */
		void ClearIndicatorClampedStatusChangedFlag() const
		{
			bWasIndicatorClampedStatusChanged = false;
		}

	protected:
		/** Called to refresh the visibility of the indicator */
		void RefreshVisibility()
		{
			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;
			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
		}

	private:
		/** Raw pointer to the indicator. This is not a UPROPERTY because the indicator is kept alive by the SActorCanvas::AddReferenceObjects */
		UIndicatorDescriptor* Indicator;

		/** The screen position of the indicator */
		FVector2D ScreenPosition;

		/** The depth of the indicator */
		double Depth;

		/** The priority of the indicator. Lower values are drawn first */
		int32 Priority;

		/** Whether the indicator is visible */
		uint8 bIsIndicatorVisible : 1;

		/** Whether the indicator is in front of the camera */
		uint8 bInFrontOfCamera : 1;

		/** Whether the indicator has a valid screen position */
		uint8 bHasValidScreenPosition : 1;

		/** Whether the indicator is dirty and needs to be redrawn next frame */
		uint8 bDirty : 1;

		/** 
		 * Cached & frame-deferred value of whether the indicator was visually screen clamped last frame or not; 
		 * Semi-hacky mutable implementation as it is cached during a const paint operation
		 */
		mutable uint8 bWasIndicatorClamped : 1;
		mutable uint8 bWasIndicatorClampedStatusChanged : 1;
	};

	/** ActorCanvas-specific slot class */
	class FArrowSlot : public TSlotBase<FArrowSlot>
	{
	public:
		FArrowSlot()
		{
		}
	};
	
	SLATE_BEGIN_ARGS(SActorCanvas)
		{
		_Visibility = EVisibility::HitTestInvisible;
		}
		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)
	SLATE_END_ARGS()
	
public:
	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InContext, const FSlateBrush* ActorCanvasArrowBrush);

	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override { return TEXT("SActorCanvas"); }
	//~ End FGCObject Interface

	//~ Begin SWidget Interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override { return FVector2D::ZeroVector; }
	virtual FChildren* GetChildren() override { return &AllChildren; }
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~ End SWidget Interface

	/** Set whether to draw elements in the order they were added to canvas. Note: Enabling this will disable batching and will cause a greater number of drawcalls */
	void SetDrawElementsInOrder(bool bInDrawElementsInOrder) { bDrawElementsInOrder = bInDrawElementsInOrder; }

private:
	/** Called when an indicator is added to the canvas */
	void OnIndicatorAdded(UIndicatorDescriptor* Indicator);

	/** Called when an indicator is removed from the canvas */
	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator);

	/** Adds a new indicator for the given entry */
	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator);

	/** Removes the indicator for the given entry */
	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator);

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	FScopedWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator);
	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget);

	/** Sets whether we want to show any indicators */
	void SetShowAnyIndicators(bool bShowAnyIndicators);

	/** Updates the canvas */
	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime);

	/** Helper function for calculating the offset */
	void GetOffsetAndSize(const UIndicatorDescriptor* Indicator,
		FVector2D& OutSize, 
		FVector2D& OutOffset,
		FVector2D& OutPaddingMin,
		FVector2D& OutPaddingMax) const;

	/** Called to update the active timer */
	void UpdateActiveTimer();

private:
	/** List of all indicators */
	TArray<TObjectPtr<UIndicatorDescriptor>> AllIndicators;

	/** List of all inactive indicators */
	TArray<UIndicatorDescriptor*> InactiveIndicators;
	
	/** Context struct for the local player owning this canvas */
	FLocalPlayerContext LocalPlayerContext;

	/** The brush to use for the indicator arrow */
	const FSlateBrush* ActorCanvasArrowBrush = nullptr;

	/** The pool of indicators. Handles the creation and destruction of the widgets */
	FUserWidgetPool IndicatorPool;

	/** List of all slots in this canvas */
	TPanelChildren<FSlot> CanvasChildren;

	/** List of all arrow slots in this canvas */
	mutable TPanelChildren<FArrowSlot> ArrowChildren;

	/** List of all combined children in this canvas */
	FCombinedChildren AllChildren;

	/** Whether to draw elements in the order they were added to canvas. Note: Enabling this will disable batching and will cause a greater number of drawcalls */
	bool bDrawElementsInOrder = false;

	/** Whether to show any indicators */
	bool bShowAnyIndicators = false;

	/** Optional geometry for painting */
	mutable TOptional<FGeometry> OptionalPaintGeometry;

	/** Weak pointer to the indicator manager component */
	TWeakObjectPtr<UIndicatorManagerComponent> IndicatorComponentPtr;

	mutable int32 NextArrowIndex = 0;
	mutable int32 ArrowIndexLastUpdate = 0;
	TSharedPtr<FActiveTimerHandle> TickHandle;
};
