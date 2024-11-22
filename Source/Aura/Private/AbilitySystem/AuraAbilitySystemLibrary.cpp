// Copyright Jordan Jackson


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilityTypes.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerState.h"
#include "Game/AuraGameModeBase.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = WorldContextObject->GetWorld()->GetFirstPlayerController())
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = WorldContextObject->GetWorld()->GetFirstPlayerController())
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, UAbilitySystemComponent* InASC, ECharacterClass CharacterClass, float Level)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)
	{
		return;
	}

	const AActor* AvatarActor = InASC->GetAvatarActor();
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryContextHandle = InASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryEffectSpecHandle = InASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
	InASC->ApplyGameplayEffectSpecToSelf(*PrimaryEffectSpecHandle.Data);

	FGameplayEffectContextHandle SecondaryContextHandle = InASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryEffectSpecHandle = InASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	InASC->ApplyGameplayEffectSpecToSelf(*SecondaryEffectSpecHandle.Data);

	FGameplayEffectContextHandle VitalContextHandle = InASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalEffectSpecHandle = InASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalContextHandle);
	InASC->ApplyGameplayEffectSpecToSelf(*VitalEffectSpecHandle.Data);
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)
	{
		return;
	}

	TScriptInterface<ICombatInterface> CombatInterface = ASC->GetAvatarActor();
	
	const int32 PowerLevel = CombatInterface.GetInterface() ? CombatInterface->GetPowerLevel() : 1;

	for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, PowerLevel);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.ClassAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, PowerLevel);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if (!AuraGameMode)
	{
		return nullptr;
	}

	return AuraGameMode->CharacterClassInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> OverlapResults;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(OverlapResults, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams::InitType::AllDynamicObjects, FCollisionShape::MakeSphere(Radius), SphereParams);
	}
	for (FOverlapResult& Overlap : OverlapResults)
	{
		const bool bIsAlive = Overlap.GetActor()->Implements<UCombatInterface>() ? !ICombatInterface::Execute_IsDead(Overlap.GetActor()) : false;
		if (bIsAlive)
		{
			OutOverlappingActors.AddUnique(Overlap.GetActor());
		}
	}
}
