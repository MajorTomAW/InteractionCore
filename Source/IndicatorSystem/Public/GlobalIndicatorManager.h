// Copyright © 2025 Playton. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GlobalIndicatorManager.generated.h"

class UGlobalIndicatorManager;
struct FIndicatorEntryContainer;
class UIndicatorDescriptor;

/** Represents an entry in the indicator list */
USTRUCT(BlueprintType)
struct FIndicatorEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	/** The indicator descriptor */
	UPROPERTY()
	TObjectPtr<UIndicatorDescriptor> Indicator = nullptr;

	//~ Begin FFastArraySerializerItem Interface
	void PreReplicatedRemove(FIndicatorEntryContainer& InArraySerializer);
	void PostReplicatedAdd(FIndicatorEntryContainer& InArraySerializer);
	//~ End FFastArraySerializerItem Interface
};

/** Fast serializer wrapper for the indicator list */
USTRUCT(BlueprintType)
struct FIndicatorEntryContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FIndicatorEntryContainer()
		: Owner(nullptr)
	{
	}

	FIndicatorEntryContainer(UGlobalIndicatorManager* InOwner)
		: Owner(InOwner)
	{
	}

	/** List of all indicators */
	UPROPERTY()
	TArray<FIndicatorEntry> Entries;

	/** Component that owns this list */
	UPROPERTY(NotReplicated)
	TObjectPtr<UGlobalIndicatorManager> Owner;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FIndicatorEntry, FIndicatorEntryContainer>(Entries, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FIndicatorEntryContainer> : public TStructOpsTypeTraitsBase2<FIndicatorEntryContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};



/**
 * Manager component that manages indicators on a local players screen
 * Used as a "middleman" to make sure that indicators are properly replicated to all clients
 */
UCLASS(ClassGroup=(IndicatorSystem))
class INDICATORSYSTEM_API UGlobalIndicatorManager : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGlobalIndicatorManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Static function to get the global indicator manager component for a given world context object */
	static UGlobalIndicatorManager* Get(const UObject* WorldContextObject);

	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
	virtual void OnAddIndicator(FIndicatorEntry& Entry);
	void AddReplicatedIndicator(UIndicatorDescriptor* IndicatorDescriptor);
	void MarkIndicatorEntryDirty(FIndicatorEntry& Entry, bool bWasAddOrChange = false);

protected:
	//~ Begin UGameStateComponent Interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList) override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void PreNetReceive() override;
	virtual void PostNetReceive() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetReplicatedCustomConditionState(FCustomPropertyConditionState& OutActiveState) const override;
	//~ End UGameStateComponent Interface

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Indicators)
	FIndicatorEntryContainer IndicatorContainer;

	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> AllReplicatedIndicators;

	const TArray<UIndicatorDescriptor*>& GetReplicatedIndicators() const { return AllReplicatedIndicators; }
	TArray<TObjectPtr<UIndicatorDescriptor>>& GetReplicatedIndicators() { return AllReplicatedIndicators; }

	UFUNCTION()
	void OnRep_Indicators() {}
};
