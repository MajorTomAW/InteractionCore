// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.


#include "IndicatorLibrary.h"

#include "IndicatorDescriptor.h"
#include "IndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

UIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return UIndicatorManagerComponent::FindIndicatorManagerComponent(Controller);
}

UIndicatorDescriptor* UIndicatorLibrary::CreateIndicator(
	TSubclassOf<UIndicatorDescriptor> IndicatorClass, USceneComponent* Component, FName ComponentSocketName, UIndicatorManagerComponent* Manager, UObject* DataObject)
{
	if (IndicatorClass == nullptr)
	{
		return nullptr;
	}

	UIndicatorDescriptor* Indicator = NewObject<UIndicatorDescriptor>(Manager, IndicatorClass);
	check(Indicator);

	Indicator->SetManagerComponent(Manager);
	Indicator->SetDataObject(DataObject);
	Indicator->SetSceneComponent(Component);
	Indicator->SetComponentSocketName(ComponentSocketName);

	return Indicator;
}
