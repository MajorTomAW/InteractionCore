// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"

#include "GameplayAbilityTargetActor_Interact.generated.h"

class AActor;
class UObject;

/** Intermediate base class for all interaction target actors. */
UCLASS(Blueprintable)
class INTERACTIONCORE_API AGameplayAbilityTargetActor_Interact : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin AGameplayAbilityTargetActor_Trace Interface
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
	//~ End AGameplayAbilityTargetActor_Trace Interface
};
