// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.


#include "IndicatorLibrary.h"

#include "IndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

UIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return UIndicatorManagerComponent::FindIndicatorManagerComponent(Controller);
}
