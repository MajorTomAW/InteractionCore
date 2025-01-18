// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.


#include "SActorCanvasArrowWidget.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SActorCanvasArrowWidget::Construct(const FArguments& InArgs, const FSlateBrush* ActorCanvasArrowBrush)
{
	Arrow = ActorCanvasArrowBrush;
	SetCanTick(false);
}

int32 SActorCanvasArrowWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 MaxLayerId = LayerId;

	if (Arrow == nullptr)
	{
		return MaxLayerId;
	}

	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	const FColor FinalColorAndOpacity = (InWidgetStyle.GetColorAndOpacityTint() * Arrow->GetTint(InWidgetStyle)).ToFColor(true);

	FSlateDrawElement::MakeRotatedBox
	(
		OutDrawElements,
		MaxLayerId++,
		AllottedGeometry.ToPaintGeometry(Arrow->ImageSize, FSlateLayoutTransform()),
		Arrow,
		DrawEffects,
		FMath::DegreesToRadians(GetRotation()),
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		FinalColorAndOpacity
	);

	return MaxLayerId;
}

FVector2D SActorCanvasArrowWidget::ComputeDesiredSize(float) const
{
	if (Arrow)
	{
		return Arrow->ImageSize;
	}
	
	return FVector2D::ZeroVector;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
