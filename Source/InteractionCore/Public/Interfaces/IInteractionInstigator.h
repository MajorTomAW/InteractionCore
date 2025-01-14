// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionOption.h"
#include "UObject/Interface.h"
#include "IInteractionInstigator.generated.h"

struct FInteractionQuery;

/**
 * Interface for an object that can instigate interactions.
 * Should be implemented by the actor that is responsible for starting interactions.
 *
 * Allows for an actor to add an arbitrator to the interaction process.
 * For example, some games present the user with a menu to pick which interaction they want to perform.
 * This will allow you to take the multiple matches (Assuming your InteractionOption generates more than one option).
 */
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class INTERACTIONCORE_API UInteractionInstigator : public UInterface
{
	GENERATED_BODY()
};


class INTERACTIONCORE_API IInteractionInstigator
{
	GENERATED_BODY()

public:
	/** Will be called if there are more than one InteractOptions that need to be decided on. */
	virtual FInteractionOption ChooseBestInteractionOption(const FInteractionQuery& Query, const TArray<FInteractionOption>& Options) = 0;
};
