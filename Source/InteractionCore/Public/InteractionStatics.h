// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "InteractionStatics.generated.h"

template <typename InterfaceType> class TScriptInterface;

class AActor;
class IInteractableTarget;
class UObject;
struct FFrame;
struct FHitResult;
struct FOverlapResult;

/**
 * Statics library for interaction-related functions.
 */
UCLASS()
class INTERACTIONCORE_API UInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UInteractionStatics();

	/** Returns the actor from an interactable target interface. */
	UFUNCTION(BlueprintCallable, Category = Interaction)
	static AActor* GetActorFromInteractableTarget(const TScriptInterface<IInteractableTarget>& InteractableTarget);

	/** Returns all interactable targets from a given actor. */
	UFUNCTION(BlueprintCallable, Category = Interaction)
	static void GetInteractableTargetsFromActor(AActor* Actor, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

public:
	static void AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
	static void AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
};
