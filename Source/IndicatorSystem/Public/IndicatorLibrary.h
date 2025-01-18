// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "IndicatorLibrary.generated.h"

class AController;
class UIndicatorManagerComponent;
class UObject;
struct FFrame;

UCLASS()
class INDICATORSYSTEM_API UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Find the Indicator Manager Component for the given Controller */
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static UIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};
