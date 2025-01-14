// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "InteractionQuery.generated.h"

/** Defines a single query for interaction */
USTRUCT(BlueprintType)
struct FInteractionQuery
{
	GENERATED_BODY()

public:
	/** The pawn requesting the interaction */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<APawn> RequestingPawn;

	/** The controller requesting the interaction (doesn't need to match the owner of the requesting pawn) */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;

	/** A generic UObject to store extra data required for the interaction */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};
