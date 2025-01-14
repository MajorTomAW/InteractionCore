// Copyright © 2024 MajorT. All Rights Reserved.


#include "Abilities/GameplayAbilityTargetActor_Interact.h"

#include "GameFramework/LightWeightInstanceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbilityTargetActor_Interact)

AGameplayAbilityTargetActor_Interact::AGameplayAbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FHitResult AGameplayAbilityTargetActor_Interact::PerformTrace(AActor* InSourceActor)
{
	constexpr bool bTraceComplex = false;
	const TArray ActorsToIgnore = { InSourceActor };

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_Interact), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	const FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;

	// Effective on the server and local client only
	AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);

	FHitResult Hit;
	LineTraceWithFilter(Hit, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);

	// Default to the end of the trace line if we don't hit anything.
	if (!Hit.bBlockingHit)
	{
		Hit.Location = TraceEnd;
	}

	if (AGameplayAbilityWorldReticle* Reticle = ReticleActor.Get())
	{
		const bool bHitActor = (Hit.bBlockingHit && Hit.HitObjectHandle.IsValid());
		const FVector ReticleLocation = (bHitActor && Reticle->bSnapToTargetedActor)
			? FLightWeightInstanceSubsystem::Get().GetLocation(Hit.HitObjectHandle)
			: Hit.Location;

		Reticle->SetActorLocation(ReticleLocation);
		Reticle->SetIsTargetAnActor(bHitActor);
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green);
		DrawDebugSphere(GetWorld(), TraceEnd, 100.0f, 16, FColor::Green);
	}
#endif

	return Hit;
}
