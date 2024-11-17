// Copyright Jordan Jackson


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!TargetASC)
	{
		return;
	}
	check(GameplayEffectClass);

	if (!bApplyEffectsToEnemies && TargetActor->ActorHasTag(FName("Enemy")))
	{
		return;
	}
	
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel, EffectContextHandle);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	EGameplayEffectDurationType DurationType = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy;

	if (bDestroyOnEffectApplication && DurationType != EGameplayEffectDurationType::Infinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (!bApplyEffectsToEnemies && TargetActor->ActorHasTag(FName("Enemy")))
	{
		return;
	}
	for (const FEffectApplication& EffectApplication : EffectsToApply)
	{
		if (EffectApplication.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, EffectApplication.GameplayEffectClass);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{	
	if (!bApplyEffectsToEnemies && TargetActor->ActorHasTag(FName("Enemy")))
	{
		return;
	}
	for (const FEffectApplication& EffectApplication : EffectsToApply)
	{
		if (EffectApplication.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, EffectApplication.GameplayEffectClass);
		}
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))
	{
		return;
	}

	for (const FEffectApplication& EffectApplication : EffectsToApply)
	{
		if (EffectApplication.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			TargetASC->RemoveActiveGameplayEffectBySourceEffect(EffectApplication.GameplayEffectClass, nullptr);
		}
	}
}
