// Copyright © 2024 MajorT. All Rights Reserved.


#include "Tasks/AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "InteractionQuery.h"
#include "InteractionStatics.h"
#include "Interfaces/IInteractableTarget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_GrantNearbyInteraction)

UAbilityTask_GrantNearbyInteraction::UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
	UGameplayAbility* OwningAbility, ECollisionChannel Channel, float InteractionScanRange, float InteractionScanRate, bool bShowDebug)
{
	UAbilityTask_GrantNearbyInteraction* NewTask = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	NewTask->InteractionScanRange = InteractionScanRange;
	NewTask->InteractionScanRate = InteractionScanRate;
	NewTask->Channel = Channel;
	NewTask->bShowDebug = bShowDebug;
	return NewTask;
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();

	const UWorld* World = GetWorld();
	check(World);

	World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	AActor* ActorOwner = GetAvatarActor();
	if (ActorOwner == nullptr)
	{
		return;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByChannel(OUT OverlapResults, ActorOwner->GetActorLocation(), FQuat::Identity, Channel, FCollisionShape::MakeSphere(InteractionScanRange), Params);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		if (OverlapResults.Num() > 0)
		{
			DrawDebugSphere(World, ActorOwner->GetActorLocation(), InteractionScanRange, 24, FColor::Red, false, InteractionScanRate);
		}
		else
		{
			DrawDebugSphere(World, ActorOwner->GetActorLocation(), InteractionScanRange, 24, FColor::Cyan, false, InteractionScanRate);	
		}
	}
#endif

	if (OverlapResults.Num() > 0)
	{
		TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
		UInteractionStatics::AppendInteractableTargetsFromOverlapResults(OverlapResults, OUT InteractableTargets);

		FInteractionQuery InteractionQuery;
		InteractionQuery.RequestingController = Cast<AController>(ActorOwner->GetOwner());
		InteractionQuery.RequestingPawn = Cast<APawn>(ActorOwner);

		TArray<FInteractionOption> InteractOptions;
		for (TScriptInterface<IInteractableTarget>& Interactable : InteractableTargets)
		{
			FInteractionOptionsBuilder Builder(Interactable, InteractOptions);
			Interactable->GatherInteractionOptions(InteractionQuery, Builder);
		}

		// Check if any of the options need ot grand an ability to the user before being used.
		for (FInteractionOption& Option : InteractOptions)
		{
			if (Option.InteractionAbilityToGrant)
			{
				FObjectKey ObjectKey(Option.InteractionAbilityToGrant);
				if (!InteractionAbilityCache.Find(ObjectKey))
				{
					FGameplayAbilitySpec Spec(Option.InteractionAbilityToGrant, 1, INDEX_NONE, this);
					FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
					InteractionAbilityCache.Add(ObjectKey, Handle);
				}
			}
		}
	}
}
