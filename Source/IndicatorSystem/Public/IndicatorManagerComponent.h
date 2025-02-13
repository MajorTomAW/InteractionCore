// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"

#include "IndicatorManagerComponent.generated.h"

class AController;
class UIndicatorDescriptor;
class UObject;
struct FFrame;

/**
 * Manager component that manages indicators on a local players screen
 */
UCLASS(ClassGroup=(IndicatorSystem), BlueprintType, Blueprintable)
class INDICATORSYSTEM_API UIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	DECLARE_EVENT_OneParam(UIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)

public:
	UIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Static function to get the indicator manager component for a given controller */
	static UIndicatorManagerComponent* FindIndicatorManagerComponent(AController* Controller);

	//-------------------------------------------------------------------------
	// Indicator Management
	//-------------------------------------------------------------------------

	/** Adds a new indicator to the manager */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	/** Removes an indicator from the manager */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	/** Gets the delegate for when an indicator is added */
	FIndicatorEvent& GetOnIndicatorAdded() { return OnIndicatorAdded; }

	/** Gets the delegate for when an indicator is removed */
	FIndicatorEvent& GetOnIndicatorRemoved() { return OnIndicatorRemoved; }

	/** Gets the list of indicators that this manager is managing */
	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

protected:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

protected:
	/** Delegate for when an indicator is added */
	FIndicatorEvent OnIndicatorAdded;

	/** Delegate for when an indicator is removed */
	FIndicatorEvent OnIndicatorRemoved;

private:
	/** The list of indicators that this manager is managing */
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};
