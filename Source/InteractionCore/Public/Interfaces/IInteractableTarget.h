// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionOption.h"
#include "UObject/Interface.h"

#include "IInteractableTarget.generated.h"

struct FGameplayEventData;
struct FGameplayTag;
struct FInteractionQuery;
class IInteractableTarget;

/** Interaction options builder that generates all possible interaction options for a given target */
class FInteractionOptionsBuilder
{
public:
	FInteractionOptionsBuilder(
		const TScriptInterface<IInteractableTarget>& InteractableTarget
		, TArray<FInteractionOption>& InteractOptions)
		: Interactable(InteractableTarget)
		, Options(InteractOptions)
	{
	}

	/** Adds a single interaction option to the list of options */
	void AddInteractionOptions(const FInteractionOption& Option) const
	{
		FInteractionOption& OptionEntry = Options.Add_GetRef(Option);
		OptionEntry.InteractableTarget = Interactable;
	}

private:
	TScriptInterface<IInteractableTarget> Interactable;
	TArray<FInteractionOption>& Options;
};

/**
 * Interface for generic player interaction
 * Should be implemented by any actor that can be interacted with
 */
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class INTERACTIONCORE_API UInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

class INTERACTIONCORE_API IInteractableTarget
{
	GENERATED_BODY()

public:
	/** Called to gather all possible interaction options for the target */
	virtual void GatherInteractionOptions(const FInteractionQuery& Query, FInteractionOptionsBuilder& OptionsBuilder) = 0;

	/** Called to customize the interaction event data to be sent when the interaction is performed */
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) { }
};
