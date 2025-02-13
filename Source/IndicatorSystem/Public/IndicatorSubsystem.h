// Copyright © 2025 Playton. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "IndicatorSubsystem.generated.h"

class AController;
class UIndicatorDescriptor;
class UIndicatorManagerComponent;
class UObject;
struct FFrame;

/** Global subsystem for managing indicators for all players. */
UCLASS(Config = Game)
class INDICATORSYSTEM_API UIndicatorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UIndicatorSubsystem();
	static UIndicatorSubsystem* Get(const UObject* WorldContextObject);

	//~ Begin UWorldSubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End UWorldSubsystem Interface

	/**
	 * Registers an indicator manager with this subsystem.
	 *
	 * @param IndicatorManager The indicator manager to register.
	 * @param bAddPendingIndicators If true, any pending indicators will be added to the manager.
	 */
	virtual void RegisterIndicatorManager(UIndicatorManagerComponent* IndicatorManager, bool bAddPendingIndicators = true);

	/** Unregisters an indicator manager with this subsystem. */
	virtual void UnregisterIndicatorManager(UIndicatorManagerComponent* IndicatorManager);

	/** Registers an indicator for all known players. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem, NetMulticast, Reliable)
	void RegisterIndicatorToAll(UIndicatorDescriptor* Indicator);

	/** Unregisters an indicator for all known players. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void UnregisterIndicatorFromAll(UIndicatorDescriptor* Indicator);

private:
	/** List of all registered indicator managers */
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorManagerComponent>> RegisteredIndicatorManagers;

	/** List of all active/pending indicators */
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> GloballyRegisteredIndicators;
};