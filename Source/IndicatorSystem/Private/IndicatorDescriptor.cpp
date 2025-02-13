// Copyright © 2024 MajorT. All Rights Reserved.


#include "IndicatorDescriptor.h"

#include "IndicatorManagerComponent.h"
#include "Engine/LocalPlayer.h"
#include "SceneView.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorDescriptor)

//////////////////////////////////////////////////////////////////////////
/// FIndicatorProjection

bool FIndicatorProjection::Project(
	const UIndicatorDescriptor& IndicatorDescriptor,
	const FSceneViewProjectionData& InProjectionData,
	const FVector2f& ScreenSize, FVector& OutScreenPositionWithDepth)
{
	USceneComponent* SceneComp = IndicatorDescriptor.GetSceneComponent();
	if (SceneComp == nullptr)
	{
		return false;
	}

	TOptional<FVector> WorldLocation;
	if (IndicatorDescriptor.GetComponentSocketName() != NAME_None)
	{
		WorldLocation = SceneComp->GetSocketTransform(IndicatorDescriptor.GetComponentSocketName()).GetLocation();
	}
	else
	{
		WorldLocation = SceneComp->GetComponentLocation();
	}

	const FVector ProjectWorldLocation = WorldLocation.GetValue() + IndicatorDescriptor.GetWorldPositionOffset();
	const EActorCanvasProjectionMode ProjectionMode = IndicatorDescriptor.GetProjectionMode();

	switch (ProjectionMode) {
	case EActorCanvasProjectionMode::ComponentPoint:
		{
			if (WorldLocation.IsSet())
			{
				FVector2D OutScreenSpacePosition;
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectWorldLocation, OutScreenSpacePosition, &ScreenSize);

				OutScreenSpacePosition.X += IndicatorDescriptor.GetScreenSpaceOffset().X * (bInFrontOfCamera ? 1 : -1);
				OutScreenSpacePosition.Y += IndicatorDescriptor.GetScreenSpaceOffset().Y;

				if (!bInFrontOfCamera &&
					(FBox2f(FVector2f::Zero(), ScreenSize).IsInside((FVector2f)OutScreenSpacePosition)))
				{
					const FVector2f CenterToPosition = (FVector2f(OutScreenSpacePosition) - (ScreenSize / 2)).GetSafeNormal();
					OutScreenSpacePosition = FVector2D((ScreenSize / 2) + CenterToPosition * ScreenSize);
				}

				OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y, FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation));
				return true;
			}

			return false;
		}
		
	case EActorCanvasProjectionMode::ComponentScreenBoundingBox:
	case EActorCanvasProjectionMode::ActorBoundingBox:
		{
			FBox IndicatorBox;
			if (ProjectionMode == EActorCanvasProjectionMode::ActorBoundingBox)
			{
				IndicatorBox = SceneComp->GetOwner()->GetComponentsBoundingBox();
			}
			else
			{
				IndicatorBox = SceneComp->Bounds.GetBox();
			}

			FVector2d LL, UR;
			const bool bInFrontOfCamera = ULocalPlayer::GetPixelBoundingBox(InProjectionData, IndicatorBox, LL, UR, &ScreenSize);
			
			const FVector& BoundingBoxAnchor = IndicatorDescriptor.GetBoundingBoxAnchor();
			const FVector2D& ScreenSpaceOffset = IndicatorDescriptor.GetScreenSpaceOffset();

			FVector ScreenPositionWithDepth;
			ScreenPositionWithDepth.X = FMath::Lerp(LL.X, UR.X, BoundingBoxAnchor.X) + ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
			ScreenPositionWithDepth.Y = FMath::Lerp(LL.Y, UR.Y, BoundingBoxAnchor.Y) + ScreenSpaceOffset.Y;
			ScreenPositionWithDepth.Z = FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation);

			const FVector2f ScreenSpacePosition = FVector2f(FVector2D(ScreenPositionWithDepth));
			if (!bInFrontOfCamera &&
				(FBox2f(FVector2f::Zero(), ScreenSize).IsInside(ScreenSpacePosition)))
			{
				const FVector2f CenterToPosition = (FVector2f(ScreenSpacePosition) - (ScreenSize / 2)).GetSafeNormal();
				const FVector2f ScreenPositionFromBehind = (ScreenSize / 2) + CenterToPosition * ScreenSize;
				ScreenPositionWithDepth.X = ScreenPositionFromBehind.X;
				ScreenPositionWithDepth.Y = ScreenPositionFromBehind.Y;
			}

			OutScreenPositionWithDepth = ScreenPositionWithDepth;
			return true;
		}

	case EActorCanvasProjectionMode::ActorScreenBoundingBox:
	case EActorCanvasProjectionMode::ComponentBoundingBox:
		{
			FBox IndicatorBox;
			if (ProjectionMode == EActorCanvasProjectionMode::ActorScreenBoundingBox)
			{
				IndicatorBox = SceneComp->GetOwner()->GetComponentsBoundingBox();
			}
			else
			{
				IndicatorBox = SceneComp->Bounds.GetBox();
			}

			const FVector ProjectBoxPoint = IndicatorBox.GetCenter() + (IndicatorBox.GetSize() * (IndicatorDescriptor.GetBoundingBoxAnchor() - FVector(0.5)));

			FVector2d OutScreenSpacePosition;
			const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectBoxPoint, OutScreenSpacePosition, &ScreenSize);
			OutScreenSpacePosition.X += IndicatorDescriptor.GetScreenSpaceOffset().X * (bInFrontOfCamera ? 1 : -1);
			OutScreenSpacePosition.Y += IndicatorDescriptor.GetScreenSpaceOffset().Y;

			if (!bInFrontOfCamera &&
				FBox2f(FVector2f::Zero(), ScreenSize).IsInside((FVector2f)OutScreenSpacePosition))
			{
				const FVector2f CenterToPosition = (FVector2f(OutScreenSpacePosition) - (ScreenSize / 2)).GetSafeNormal();
				OutScreenSpacePosition = FVector2D((ScreenSize / 2) + CenterToPosition * ScreenSize);
			}

			OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y, FVector::Dist(InProjectionData.ViewOrigin, ProjectBoxPoint));
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/// UIndicatorDescriptor

UIndicatorDescriptor::UIndicatorDescriptor()
{
}

UWorld* UIndicatorDescriptor::GetWorld() const
{
	return GetOuter()->GetWorld();
}

int32 UIndicatorDescriptor::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject) || !IsSupportedForNetworking())
	{
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}

	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UIndicatorDescriptor::CallRemoteFunction(
	UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	// Check for GC
	if (!IsValid(this))
	{
		return false;
	}

	// We can't replicate CDOs
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != nullptr);

	AActor* Owner = CastChecked<AActor>(GetOuter());
	bool bProcessed = false;

	FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
	if (Context != nullptr)
	{
		for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
		{
			if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(Owner, Function))
			{
				Driver.NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
				bProcessed = true;
			}
		}
	}

	return bProcessed;
}

bool UIndicatorDescriptor::IsSupportedForNetworking() const
{
	return GetShouldReplicate();
}

void UIndicatorDescriptor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WorldPositionOffset);
	DOREPLIFETIME(ThisClass, Component);
}

void UIndicatorDescriptor::UnregisterIndicator()
{
	if (UIndicatorManagerComponent* Manager = ManagerPtr.Get())
	{
		Manager->RemoveIndicator(this);
	}
}

void UIndicatorDescriptor::SetManagerComponent(UIndicatorManagerComponent* InManager)
{
	// Make sure nobody has set this.
	if (ensure(ManagerPtr.IsExplicitlyNull()))
	{
		ManagerPtr = InManager;
	}
}
