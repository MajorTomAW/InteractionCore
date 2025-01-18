// Copyright © 2024 MajorT. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widgets/SLeafWidget.h"

struct FSlateBrush;

/** A widget that displays an arrow in the actor canvas */
class SActorCanvasArrowWidget : public SLeafWidget
{
public:
	SActorCanvasArrowWidget()
		: Rotation(0.0f)
		, Arrow(nullptr)
	{
	}
	
	SLATE_BEGIN_ARGS(SActorCanvasArrowWidget)
		{
		}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const FSlateBrush* ActorCanvasArrowBrush);

	//~ Begin SWidget Interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	//~ End SWidget Interface

	/** Returns the rotation of the arrow */
	FORCEINLINE float GetRotation() const
	{
		return Rotation;
	}

	/** Sets the rotation of the arrow */
	FORCEINLINE void SetRotation(float InRotation)
	{
		Rotation = FMath::Fmod(InRotation, 360.0f);
	}

private:
	/** The rotation of the arrow (in degrees) */
	float Rotation;

	/** The brush to use for the arrow */
	const FSlateBrush* Arrow;
};
