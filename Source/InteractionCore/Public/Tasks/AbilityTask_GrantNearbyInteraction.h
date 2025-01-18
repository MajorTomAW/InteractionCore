// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_GrantNearbyInteraction.generated.h"

class UGameplayAbility;
class UObject;
struct FFrame;
struct FGameplayAbilitySpecHandle;
struct FObjectKey;

UCLASS()
class INTERACTIONCORE_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Waits until an overlap occurs. This will need to be better fleshed out, so we can specify game-specific collision requirements */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, ECollisionChannel Channel, float InteractionScanRange, float InteractionScanRate, bool bShowDebug = false);

	//~ Begin UAbilityTask Interface
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UAbilityTask Interface

protected:
	/** Called to query for interactables */
	void QueryInteractables();

	/** The interaction scan range to use for the line trace */
	float InteractionScanRange = 0.f;

	/** The interaction scan rate for the interaction */
	float InteractionScanRate = 0.1f;

	/** Whether to draw debug information */
	bool bShowDebug = false;

	/** The collision channel to use for the line trace */
	ECollisionChannel Channel = ECC_Camera;

	FTimerHandle QueryTimerHandle;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
