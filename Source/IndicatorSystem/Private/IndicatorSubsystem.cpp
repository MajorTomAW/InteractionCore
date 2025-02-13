// Copyright © 2025 Playton. All Rights Reserved.


#include "IndicatorSubsystem.h"

#include "IndicatorDescriptor.h"
#include "IndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorSubsystem)

UIndicatorSubsystem::UIndicatorSubsystem()
{
}

UIndicatorSubsystem* UIndicatorSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(World);

	return UWorld::GetSubsystem<UIndicatorSubsystem>(World);
}

bool UIndicatorSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	// We don't want to create the subsystem if there are any derived classes
	// This is to prevent multiple subsystems from being created
	TArray<UClass*> DerivedClasses;
	GetDerivedClasses(GetClass(), DerivedClasses, false);
	return DerivedClasses.Num() == 0;
}

void UIndicatorSubsystem::RegisterIndicatorManager(
	UIndicatorManagerComponent* IndicatorManager, bool bAddPendingIndicators)
{
	check(IndicatorManager);
	if (RegisteredIndicatorManagers.Contains(IndicatorManager))
	{
		return;
	}

	if (bAddPendingIndicators &&
		IndicatorManager->IsLocalController())
	{
		for (UIndicatorDescriptor* Indicator : GloballyRegisteredIndicators)
		{
			IndicatorManager->AddIndicator(Indicator);
		}
	}

	RegisteredIndicatorManagers.AddUnique(IndicatorManager);
}

void UIndicatorSubsystem::UnregisterIndicatorManager(UIndicatorManagerComponent* IndicatorManager)
{
	check(IndicatorManager);

	for (UIndicatorDescriptor* Indicator : GloballyRegisteredIndicators)
	{
		IndicatorManager->RemoveIndicator(Indicator);
	}

	RegisteredIndicatorManagers.Remove(IndicatorManager);
}

void UIndicatorSubsystem::RegisterIndicatorToAll_Implementation(UIndicatorDescriptor* Indicator)
{
	if (Indicator == nullptr)
	{
		return;
	}

	if (!GloballyRegisteredIndicators.Contains(Indicator))
	{
		GloballyRegisteredIndicators.Add(Indicator);
		for (UIndicatorManagerComponent* IndicatorManager : RegisteredIndicatorManagers)
		{
			if (!IndicatorManager->IsLocalController())
			{
				continue;
			}
			
			IndicatorManager->AddIndicator(Indicator);
		}
	}
}

void UIndicatorSubsystem::UnregisterIndicatorFromAll(UIndicatorDescriptor* Indicator)
{
	if (Indicator == nullptr)
	{
		return;
	}

	if (GloballyRegisteredIndicators.Contains(Indicator))
	{
		for (UIndicatorManagerComponent* IndicatorManager : RegisteredIndicatorManagers)
		{
			if (!IndicatorManager->IsLocalController())
			{
				continue;
			}
			
			IndicatorManager->RemoveIndicator(Indicator);
		}
		GloballyRegisteredIndicators.Remove(Indicator);
	}
}
