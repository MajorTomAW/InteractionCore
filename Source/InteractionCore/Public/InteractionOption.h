// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbility.h"

#include "InteractionOption.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UUserWidget;
class IInteractableTarget;

/** Interaction option used to define a single possible option that can be executed when interaction with this entity. */
USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

public:
	/** The interactable target */
	UPROPERTY(BlueprintReadWrite, Category = Interaction)
	TScriptInterface<IInteractableTarget> InteractableTarget;

	/** Simple text the interaction might return */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	FText Text;

	/** Simple subtext the interaction might return */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	FText SubText;

	//////////////////////////////////////////////////////////////////////////
	/// METHODS OF INTERACTION
	/// 1) Place an ability on the avatar that they can activate when they perform interaction.
	/// 2) Allow the object we're interacting with to have its own ability system and interaction ability, that we can activate instead.
	
	/// 1)
	/** The ability to grant the avatar when they get near interactable objects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	// - OR -
	/** The ability system on the target that can be used for the TargetInteractionHandle and sending the event, if needed. */
	UPROPERTY(BlueprintReadOnly, Category = Interaction)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem = nullptr;

	/** The ability spec to activate on the object for this option. */
	UPROPERTY(BlueprintReadOnly, Category = Interaction)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;

	/** The widget to show for this kind of interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:
	FORCEINLINE bool operator==(const FInteractionOption& Other) const
	{
		return InteractableTarget == Other.InteractableTarget &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant &&
			TargetAbilitySystem == Other.TargetAbilitySystem &&
			TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle &&
			InteractionWidgetClass == Other.InteractionWidgetClass;
	}

	FORCEINLINE bool operator!=(const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FInteractionOption& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}

	FORCEINLINE friend uint32 GetTypeHash(FInteractionOption const& This)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(This.InteractableTarget));
		Hash = HashCombine(Hash, GetTypeHash(This.InteractionAbilityToGrant));
		Hash = HashCombine(Hash, GetTypeHash(This.TargetAbilitySystem));
		Hash = HashCombine(Hash, GetTypeHash(This.TargetInteractionAbilityHandle));
		Hash = HashCombine(Hash, GetTypeHash(This.InteractionWidgetClass));
		return Hash;
	}
};
