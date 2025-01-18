// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "IndicatorLibrary.generated.h"

class UIndicatorDescriptor;
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = IndicatorSystem)
	static UIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);

	/** Creates a new indicator from the given class. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem, meta = (AdvancedDisplay = 3, DeterminesOutputType = "IndicatorClass"))
	static UIndicatorDescriptor* CreateIndicator(TSubclassOf<UIndicatorDescriptor> IndicatorClass, USceneComponent* TargetComponent, FName TargetSocketName, UIndicatorManagerComponent* OwnerManager, UObject* DataObject = nullptr);
};
