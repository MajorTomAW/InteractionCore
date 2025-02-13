// Copyright © 2025 Playton. All Rights Reserved.


#include "GlobalIndicatorManager.h"

#include "IndicatorDescriptor.h"
#include "IndicatorManagerComponent.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GlobalIndicatorManager)

//////////////////////////////////////////////////////////////////////////
/// FIndicatorEntry

void FIndicatorEntry::PreReplicatedRemove(FIndicatorEntryContainer& InArraySerializer)
{
	
}

void FIndicatorEntry::PostReplicatedAdd(FIndicatorEntryContainer& InArraySerializer)
{
	if (InArraySerializer.Owner)
	{
		InArraySerializer.Owner->OnAddIndicator(*this);
	}
}

//////////////////////////////////////////////////////////////////////////
/// FIndicatorEntryContainer

//////////////////////////////////////////////////////////////////////////
/// UGlobalIndicatorManager

UGlobalIndicatorManager::UGlobalIndicatorManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, IndicatorContainer(this)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;

	SetIsReplicatedByDefault(true);
}

UGlobalIndicatorManager* UGlobalIndicatorManager::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(World);

	return World->GetGameState()->GetComponentByClass<UGlobalIndicatorManager>();
}

void UGlobalIndicatorManager::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	FIndicatorEntry& Entry = IndicatorContainer.Entries.AddDefaulted_GetRef();
	Entry.Indicator = IndicatorDescriptor;

	if (!IsValid(Entry.Indicator))
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	AddReplicatedIndicator(Entry.Indicator);

	OnAddIndicator(Entry);
	MarkIndicatorEntryDirty(Entry, true);
}

void UGlobalIndicatorManager::AddReplicatedIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	TArray<TObjectPtr<UIndicatorDescriptor>>& ReplicatedIndicators = GetReplicatedIndicators();
	if (ReplicatedIndicators.Find(IndicatorDescriptor) == INDEX_NONE)
	{
		ReplicatedIndicators.Add(IndicatorDescriptor);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(IndicatorDescriptor);
		}
	}
}

void UGlobalIndicatorManager::OnAddIndicator(FIndicatorEntry& Entry)
{
	if (!Entry.Indicator)
	{
		return;
	}

	for (APlayerState* PS : GetGameState<AGameStateBase>()->PlayerArray)
	{
		APlayerController* PC = PS->GetPlayerController();
		if (!PC)
		{
			continue;
		}

		if (UIndicatorManagerComponent* IndicatorManager = UIndicatorManagerComponent::FindIndicatorManagerComponent(PC))
		{
			IndicatorManager->AddIndicator(Entry.Indicator);
		}
	}
}

void UGlobalIndicatorManager::MarkIndicatorEntryDirty(FIndicatorEntry& Entry, bool bWasAddOrChange)
{
	if (HasAuthority())
	{
		if (Entry.Indicator == nullptr || bWasAddOrChange)
		{
			IndicatorContainer.MarkItemDirty(Entry);
		}
		else
		{
			IndicatorContainer.MarkArrayDirty();
		}
	}
	else
	{
		IndicatorContainer.MarkArrayDirty();
	}
}

void UGlobalIndicatorManager::InitializeComponent()
{
	Super::InitializeComponent();
}

void UGlobalIndicatorManager::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UGlobalIndicatorManager::OnRegister()
{
	Super::OnRegister();
}

void UGlobalIndicatorManager::OnUnregister()
{
	Super::OnUnregister();
}

void UGlobalIndicatorManager::BeginPlay()
{
	Super::BeginPlay();
}

void UGlobalIndicatorManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UGlobalIndicatorManager::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList)
{
	Super::GetSubobjectsWithStableNamesForNetworking(ObjList);
}

bool UGlobalIndicatorManager::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
#if SUBOBJECT_TRANSITION_VALIDATION
	// When true it means we are calling this function to find any leftover replicated subobjects in classes that transitioned to the new registry list.
	// This shared class needs to keep supporting the old ways until we fully deprecate the API, so by only returning false we prevent the ensures to trigger
	if (UActorChannel::CanIgnoreDeprecatedReplicateSubObjects())
	{
		return false;
	}
#endif

	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UIndicatorDescriptor* Indicator : GetReplicatedIndicators())
	{
		if (IsValid(Indicator))
		{
			WroteSomething |= Channel->ReplicateSubobject(Indicator, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UGlobalIndicatorManager::PreNetReceive()
{
	Super::PreNetReceive();
}

void UGlobalIndicatorManager::PostNetReceive()
{
	Super::PostNetReceive();
}

void UGlobalIndicatorManager::ReadyForReplication()
{
	Super::ReadyForReplication();
}

void UGlobalIndicatorManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// Fast Arrays don't use push model, but there's no harm in marking them with it.
	// The flag will just be ignored.
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, IndicatorContainer, Params);
	
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UGlobalIndicatorManager::GetReplicatedCustomConditionState(FCustomPropertyConditionState& OutActiveState) const
{
	Super::GetReplicatedCustomConditionState(OutActiveState);
}
