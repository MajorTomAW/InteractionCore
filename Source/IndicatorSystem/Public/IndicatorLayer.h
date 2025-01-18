// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "Components/Widget.h"

#include "IndicatorLayer.generated.h"

class AActor;
class UObject;
class SWidget;
class SActorCanvas;
struct FSlateBrush;

/** A layer that can be used to render indicators on top of the game world. */
UCLASS()
class INDICATORSYSTEM_API UIndicatorLayer : public UWidget
{
	GENERATED_BODY()

public:
	UIndicatorLayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Default arrow brush to use if UI is clamped to the screen and needs to show an arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateBrush ArrowBrush;

protected:
	//~ Begin UWidget Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

protected:
	/** The actor canvas that will render the indicators. */
	TSharedPtr<SActorCanvas> MyActorCanvas;
};
