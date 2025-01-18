// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Engine/CollisionProfile.h"
#include "InteractionOption.h"
#include "InteractionQuery.h"

#include "AbilityTask_WaitForInteractableTargets.generated.h"

class AActor;
class IInteractableTarget;
class UObject;
class UWorld;
struct FCollisionQueryParams;
struct FHitResult;
struct FInteractionQuery;
template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, const TArray<FInteractionOption>&, InteractableOptions);

/** Base ability task that listens for any potential interactable actors in range. */
UCLASS(Abstract)
class UAbilityTask_WaitForInteractableTargets : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_WaitForInteractableTargets(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Delegate that gets called whenever the interaction options changed */
	UPROPERTY(BlueprintAssignable)
	FInteractableObjectsChangedEvent InteractableObjectsChanged;

protected:
	/** Performs the actual line trace */
	static void LineTrace(FHitResult& OutHit, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params);
	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPos);

	/** Aims with the owning player controller */
	virtual void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& Start, float MaxRange, FVector& OutEnd, bool bIgnorePitch = false) const;

	/** Called to update current interactable options */
	virtual void UpdateInteractableOptions(const FInteractionQuery& Query, const TArray<TScriptInterface<IInteractableTarget>>& InteractableTargets);

protected:
	/** The collision profile name to use for the trace */
	FCollisionProfileName TraceProfile;

	/** Whether the trace affects the aiming pitch */
	bool bTraceAffectsAimPitch = false;

	/** Cached list of current interaction options */
	TArray<FInteractionOption> CurrentOptions;
};
