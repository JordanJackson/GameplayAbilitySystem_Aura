// Copyright Jordan Jackson


#include "AbilitySystem/Abilities/AuraProjectileGameplayAbility.h"
#include "Actor/AuraProjectile.h"
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
		Rotation.Pitch = 0.0f;	// fly parallel to ground plane
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, OwningActor, Cast<APawn>(AvatarActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		// TODO: Give the Projectile a Gameplay Effect Spec
		Projectile->FinishSpawning(SpawnTransform);
	}
}
