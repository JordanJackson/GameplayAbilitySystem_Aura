// Copyright Jordan Jackson


#include "AbilitySystem/Abilities/AuraProjectileGameplayAbility.h"
#include "Actor/AuraProjectile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

void UAuraProjectileGameplayAbility::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	const bool bIsServer = AvatarActor->HasAuthority();
	if (!bIsServer)
	{
		return;
	}

	AActor* OwningActor = GetOwningActorFromActorInfo();
	TScriptInterface<ICombatInterface> CombatInterface = AvatarActor;
	if (CombatInterface.GetInterface())
	{
		FTransform SpawnTransform;
		const FVector WeaponSocketLocation = CombatInterface->GetWeaponProjectileSocketLocation();
		SpawnTransform.SetLocation(WeaponSocketLocation);
		
		FRotator Rotation = (ProjectileTargetLocation - WeaponSocketLocation).Rotation();
		//Rotation.Pitch = 0.0f;	// fly parallel to ground plane
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, OwningActor, Cast<APawn>(AvatarActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AvatarActor);
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		// Just an example of Actors Array, HitResult
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Damage"));
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DamageTag, ScaledDamage);

		Projectile->DamageEffectSpecHandle = EffectSpecHandle;

		Projectile->FinishSpawning(SpawnTransform);
	}
}