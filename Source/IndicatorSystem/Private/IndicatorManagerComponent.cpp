// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.


#include "IndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorManagerComponent)

UIndicatorManagerComponent::UIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

UIndicatorManagerComponent* UIndicatorManagerComponent::FindIndicatorManagerComponent(AController* Controller)
{
	return Controller ? Controller->FindComponentByClass<UIndicatorManagerComponent>() : nullptr;
}

void UIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetManagerComponent() == this);

		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
