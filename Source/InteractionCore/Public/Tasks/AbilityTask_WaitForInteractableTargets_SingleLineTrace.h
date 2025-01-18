// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "InteractionQuery.h"
#include "AbilityTask_WaitForInteractableTargets.h"

#include "AbilityTask_WaitForInteractableTargets_SingleLineTrace.generated.h"

struct FCollisionProfileName;
class UGameplayAbility;
class UObject;
struct FFrame;

/** Ability task used to scan for interactable targets in a given radius. */
UCLASS()
class UAbilityTask_WaitForInteractableTargets_SingleLineTrace : public UAbilityTask_WaitForInteractableTargets
{
	GENERATED_BODY()

public:
	UAbilityTask_WaitForInteractableTargets_SingleLineTrace(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin UAbilityTask Interface
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UAbilityTask Interface

	/** Waits until we trace a new set of interactables. This task automatically loops.*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitForInteractableTargets_SingleLineTrace(UGameplayAbility* OwningAbility, FInteractionQuery InteractionQuery, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange = 100.f, float InteractionScanRate = 0.1f, bool bShowDebug = false);

protected:
	/** Performs the actual trace */
	virtual void PerformTrace();

protected:
	UPROPERTY()
	FInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;
	
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	bool bShowDebug = false;

	FTimerHandle TimerHandle;
};
