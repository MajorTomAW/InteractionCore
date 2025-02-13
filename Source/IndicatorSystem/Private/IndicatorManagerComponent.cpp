// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.


#include "IndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"
#include "IndicatorSubsystem.h"

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
	/*UE_LOG(LogTemp, Error, TEXT("New Indicator: %s\nTargetCop: %s\nOffset: %s")
		, *GetNameSafe(IndicatorDescriptor), *GetNameSafe(IndicatorDescriptor->GetSceneComponent()), *IndicatorDescriptor->GetWorldPositionOffset().ToString());*/
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

void UIndicatorManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UIndicatorSubsystem* IndicatorSub = UIndicatorSubsystem::Get(this))
	{
		IndicatorSub->RegisterIndicatorManager(this);
	}
}

void UIndicatorManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UIndicatorSubsystem* IndicatorSub = UIndicatorSubsystem::Get(this))
	{
		IndicatorSub->UnregisterIndicatorManager(this);
	}
	
	Super::EndPlay(EndPlayReason);
}
