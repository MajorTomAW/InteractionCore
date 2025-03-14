// Copyright © 2024 MajorT. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Components/SceneComponent.h"
#include "Net/Core/PushModel/PushModelMacros.h"
#include "Types/SlateEnums.h"

#include "IndicatorDescriptor.generated.h"

class UIndicatorManagerComponent;
class SWidget;
class UIndicatorDescriptor;
class UUserWidget;
struct FFrame;
struct FSceneViewProjectionData;

/** Utility struct used to project an indicator into screen space */
struct FIndicatorProjection
{
public:
	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& OutScreenPositionWithDepth);
};

/** Defines the different ways an indicator can be projected onto the screen */
UENUM(BlueprintType)
enum class EActorCanvasProjectionMode : uint8
{
	ComponentPoint,

	ComponentBoundingBox,

	ComponentScreenBoundingBox,

	ActorBoundingBox,

	ActorScreenBoundingBox
};

/**
 * Describes and controls an active indicator.  It is highly recommended that your widget implements
 * IActorIndicatorWidget so that it can 'bind' to the associated data.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject))
class INDICATORSYSTEM_API UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()
	REPLICATED_BASE_CLASS(UIndicatorDescriptor)
	
	friend class SActorCanvas;

public:
	UIndicatorDescriptor();

	//~ Begin UObject Interface
	virtual UWorld* GetWorld() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	virtual bool IsSupportedForNetworking() const override;
	//~ End UObject Interface

	/** Unregisters this indicator from the manager */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void UnregisterIndicator();

	//-------------------------------------------------------------------------
	// Accessors
	//-------------------------------------------------------------------------

	/** Returns the data object associated with this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	UObject* GetDataObject() const { return DataObject; }

	/** Sets the data object associated with this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }

	/** Returns the scene component associated with this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	USceneComponent* GetSceneComponent() const { return Component; }

	/** Sets the scene component associated with this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }

	/** Returns the name of the socket on the component to use for the indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	FName GetComponentSocketName() const { return ComponentSocketName; }

	/** Sets the name of the socket on the component to use for the indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }

	/** Returns the class of the user widget to use for the indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }

	/** Sets the class of the user widget to use for the indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		IndicatorWidgetClass = InIndicatorWidgetClass;
	}

	/** Returns whether this indicator should be automatically removed when the indicator component is null */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }

	/** Sets whether this indicator should be automatically removed when the indicator component is null */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
	{
		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
	}

	/** Returns whether this indicator can be automatically removed */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool CanAutomaticallyRemove() const
	{
		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
	}

	/** Returns whether this indicator is visible */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }

	/** Sets the desired visibility of this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetDesiredVisibility(bool InVisible)
	{
		bVisible = InVisible;
	}

	/** Returns whether this indicator should replicate to others */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool GetShouldReplicate() const { return bShouldReplicate; }

	/** Returns the projection mode for this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }

	/** Sets the projection mode for this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
	{
		ProjectionMode = InProjectionMode;
	}

	/** Returns the horizontal alignment of the indicator. Used for alignment to the point in space to place the indicator at. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	EHorizontalAlignment GetHAlign() const { return HAlignment; }

	/** Sets the horizontal alignment of the indicator. Used for alignment to the point in space to place the indicator at. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetHAlign(EHorizontalAlignment InHAlignment)
	{
		HAlignment = InHAlignment;
	}

	/** Returns the vertical alignment of the indicator. Used for alignment to the point in space to place the indicator at. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	EVerticalAlignment GetVAlign() const { return VAlignment; }

	/** Sets the vertical alignment of the indicator. Used for alignment to the point in space to place the indicator at. */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetVAlign(EVerticalAlignment InVAlignment)
	{
		VAlignment = InVAlignment;
	}

	/** Returns whether this indicator should be clamped to the edge of the screen */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool GetClampToScreen() const { return bClampToScreen; }

	/** Sets whether this indicator should be clamped to the edge of the screen */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetClampToScreen(bool bValue)
	{
		bClampToScreen = bValue;
	}

	/** Returns whether this indicator should be hidden at a certain distance */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool GetHasMaxDrawDistance() const { return bHideAtDistance; }

	/** Sets whether this indicator should be hidden at a certain distance */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetHideAtDistance(bool bValue)
	{
		bHideAtDistance = bValue;
	}

	/** Returns the distance at which this indicator should be hidden */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	float GetMaxDrawDistance() const { return MaxDrawDistance; }

	/** Sets the distance at which this indicator should be hidden */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetMaxDrawDistance(float Distance)
	{
		MaxDrawDistance = Distance;
	}

	/** Returns whether this indicator should show an arrow when clamped to the edge of the screen */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }

	/** Sets whether this indicator should show an arrow when clamped to the edge of the screen */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetShowClampToScreenArrow(bool bValue)
	{
		bShowClampToScreenArrow = bValue;
	}

	/** Returns the position offset for the indicator in world space */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }

	/** Sets the position offset for the indicator in world space */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetWorldPositionOffset(FVector Offset)
	{
		WorldPositionOffset = Offset;
	}

	/** Returns the position offset for the indicator in screen space */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }

	/** Sets the position offset for the indicator in screen space */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetScreenSpaceOffset(FVector2D Offset)
	{
		ScreenSpaceOffset = Offset;
	}

	/** Returns the bounding box anchor for the indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }

	/** Sets the bounding box anchor for the indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
	{
		BoundingBoxAnchor = InBoundingBoxAnchor;
	}

	/**
	 * Returns the priority of the indicator. Higher priority indicators are drawn on top of lower priority indicators
	 * Allows sorting the indicators (after they're sorted by depth), to allow some group of indicators to always be in front of others.
	 */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	int32 GetPriority() const { return Priority; }

	/**
	 * Sets the priority of the indicator. Higher priority indicators are drawn on top of lower priority indicators
	 * Allows sorting the indicators (after they're sorted by depth), to allow some group of indicators to always be in front of others.
	 */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

	/** Returns the manager component that owns this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	UIndicatorManagerComponent* GetManagerComponent() const { return ManagerPtr.Get(); }

	/** Sets the manager component that owns this indicator */
	UFUNCTION(BlueprintCallable, Category = IndicatorSystem)
	void SetManagerComponent(UIndicatorManagerComponent* InManager);

protected:
	/** The actual spawned widget for this indicator */
	TWeakObjectPtr<UUserWidget> IndicatorWidget;
	
	//-------------------------------------------------------------------------
	// Properties
	//-------------------------------------------------------------------------

	
	/** The class of the user widget to use for the indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;
	
	/** Determines whether this indicator is visible or not */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	bool bVisible = true;

	/** Determines whether this indicator should replicate to others */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator", AdvancedDisplay)
	bool bShouldReplicate = true;

	/** Determines whether this indicator should be clamped to the edge of the screen */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	bool bClampToScreen = false;

	/** Determines whether this indicator should show an arrow when clamped to the edge of the screen */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	bool bShowClampToScreenArrow = false;

	/** Determines whether this indicator should be hidden at a certain distance */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator", meta=(InlineEditConditionToggle))
	bool bHideAtDistance = false;

	/** The distance at which this indicator should be hidden */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator", meta=(EditCondition="bHideAtDistance",Units=cm))
	float MaxDrawDistance = 1000.0f;

	/** Determines whether this indicator should override the screen position */
	UPROPERTY()
	bool bOverrideScreenPosition = false;

	/** Determines whether this indicator should be automatically removed when the indicator component is null */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	/** The projection mode for this indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator|Projection")
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;

	/** The horizontal alignment of the indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator|Projection")
	TEnumAsByte<EHorizontalAlignment> HAlignment = EHorizontalAlignment::HAlign_Center;

	/** The vertical alignment of the indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator|Projection")
	TEnumAsByte<EVerticalAlignment> VAlignment = EVerticalAlignment::VAlign_Center;

	/** The priority of the indicator. Higher priority indicators are drawn on top of lower priority indicators */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	int32 Priority = 0;

	/** The bounding box anchor for the indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator|Positioning")
	FVector BoundingBoxAnchor = FVector(0.5f, 0.5f, 0.5f);

	/** The screen space offset for the indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator|Positioning")
	FVector2D ScreenSpaceOffset = FVector2D::ZeroVector;

	/** The world space offset for the indicator */
	UPROPERTY(EditDefaultsOnly, Category = "Indicator|Positioning", Replicated)
	FVector WorldPositionOffset = FVector::ZeroVector;

	/** The optional data object associated with this indicator */
	UPROPERTY()
	TObjectPtr<UObject> DataObject;

	/** The scene component associated with this indicator */
	UPROPERTY(Replicated)
	TObjectPtr<USceneComponent> Component;

	/** The name of the socket on the component to use for the indicator */
	UPROPERTY()
	FName ComponentSocketName = NAME_None;

	/** The manager component that owns this indicator */
	UPROPERTY()
	TWeakObjectPtr<UIndicatorManagerComponent> ManagerPtr;

	/** Canvas host for the indicator */
	TWeakPtr<SWidget> CanvasHost;

	/** Content for the indicator */
	TWeakPtr<SWidget> Content;
};
