// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/ObjectMacros.h"

#include "IActorIndicatorWidget.generated.h"

class AActor;
class UIndicatorDescriptor;

/** Interface for widgets that can display actor indicators */
UINTERFACE(BlueprintType)
class UActorIndicatorWidget : public UInterface
{
	GENERATED_BODY()
};

class INDICATORSYSTEM_API IActorIndicatorWidget
{
	GENERATED_BODY()
	
public:
	/** Bind an indicator to this widget */
	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void BindIndicator(UIndicatorDescriptor* Indicator);

	/** Unbind an indicator from this widget */
	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void UnbindIndicator(const UIndicatorDescriptor* Indicator);
};
