// Copyright © 2024 MajorT. All Rights Reserved.


#include "Tasks/AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"

#include "InteractionStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_WaitForInteractableTargets_SingleLineTrace)

UAbilityTask_WaitForInteractableTargets_SingleLineTrace::UAbilityTask_WaitForInteractableTargets_SingleLineTrace(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
}

UAbilityTask_WaitForInteractableTargets_SingleLineTrace* UAbilityTask_WaitForInteractableTargets_SingleLineTrace::
WaitForInteractableTargets_SingleLineTrace(
	UGameplayAbility* OwningAbility, FInteractionQuery InteractionQuery,
	FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation,
	float InteractionScanRange,float InteractionScanRate, bool bShowDebug)
{
	UAbilityTask_WaitForInteractableTargets_SingleLineTrace* NewTask = NewAbilityTask<UAbilityTask_WaitForInteractableTargets_SingleLineTrace>(OwningAbility);
	NewTask->InteractionScanRate = InteractionScanRate;
	NewTask->InteractionScanRange = InteractionScanRange;
	NewTask->StartLocation = StartLocation;
	NewTask->InteractionQuery = InteractionQuery;
	NewTask->TraceProfile = TraceProfile;
	NewTask->bShowDebug = bShowDebug;
	return NewTask;
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::Activate()
{
	SetWaitingOnAvatar();

	const UWorld* World = GetWorld();
	check(World);

	World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::OnDestroy(bool bInOwnerFinished)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::PerformTrace()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (Avatar == nullptr)
	{
		return;
	}

	const UWorld* World = GetWorld();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Avatar);

	constexpr bool bTraceComplex = false;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_WaitForInteractableTargets_SingleLineTrace), bTraceComplex);
	Params.AddIgnoredActors(ActorsToIgnore);

	const FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(Avatar, Params, TraceStart, InteractionScanRange, OUT TraceEnd);

	FHitResult OutHit;
	LineTrace(OutHit, World, TraceStart, TraceEnd, TraceProfile.Name, Params);

	TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
	UInteractionStatics::AppendInteractableTargetsFromHitResult(OutHit, InteractableTargets);
	
	UpdateInteractableOptions(InteractionQuery, InteractableTargets);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = OutHit.bBlockingHit ? FColor::Red : FColor::Green;
		if (OutHit.bBlockingHit)
		{
			DrawDebugLine(World, TraceStart, OutHit.Location, DebugColor, false, InteractionScanRate);
			DrawDebugSphere(World, OutHit.Location, 5, 16, DebugColor, false, InteractionScanRate);
		}
		else
		{
			DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
		}
	}
#endif
}
