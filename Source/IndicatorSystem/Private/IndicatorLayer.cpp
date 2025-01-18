// Copyright © 2024 MajorT. All Rights Reserved.


#include "IndicatorLayer.h"

#include "SActorCanvas.h"
#include "Widgets/Layout/SBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLayer)

UIndicatorLayer::UIndicatorLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	UWidget::SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyActorCanvas.Reset();
}

TSharedRef<SWidget> UIndicatorLayer::RebuildWidget()
{
	// This is a runtime-only widget
	if (!IsDesignTime())
	{
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		if (ensureMsgf(LocalPlayer, TEXT("Attempting to rebuild a UActorCanvas without a valid LocalPlayer!")))
		{
			MyActorCanvas = SNew(SActorCanvas, FLocalPlayerContext(LocalPlayer), &ArrowBrush);
			return MyActorCanvas.ToSharedRef();
		}	
	}

	// If we're in the editor, we don't want to create the actor canvas
	// Give it a trivial box, NullWidget isn't safe to use from a UWidget
	return SNew(SBox);
}
