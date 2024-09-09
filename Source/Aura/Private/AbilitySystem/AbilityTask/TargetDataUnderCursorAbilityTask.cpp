// Copyright Jordan Jackson


#include "AbilitySystem/AbilityTask/TargetDataUnderCursorAbilityTask.h"
#include "Interaction/HighlightInterface.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

UTargetDataUnderCursorAbilityTask* UTargetDataUnderCursorAbilityTask::CreateTargetDataUnderCursor(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderCursorAbilityTask* MyTaskObj = NewAbilityTask<UTargetDataUnderCursorAbilityTask>(OwningAbility);

	return MyTaskObj;
}

void UTargetDataUnderCursorAbilityTask::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendCursorTargetData();
	}
	else
	{
		const FGameplayAbilitySpecHandle AbilitySpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(AbilitySpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataSet);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(AbilitySpecHandle, ActivationPredictionKey);

		if (bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderCursorAbilityTask::SendCursorTargetData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	FHitResult CursorHit;
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (PC)
	{
		PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
		if (!CursorHit.bBlockingHit)
		{
			return;
		}

		AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(PC);

		TScriptInterface<IHighlightInterface> HighlightInterface = CursorHit.GetActor();
		if (HighlightInterface.GetInterface() || (AuraPC && AuraPC->ShouldHoldPosition()))
		{
			FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
			Data->HitResult = CursorHit;

			FGameplayAbilityTargetDataHandle TargetDataHandle;
			TargetDataHandle.Add(Data);

			AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), TargetDataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				ValidData.Broadcast(TargetDataHandle);
			}
		}
	}
}

void UTargetDataUnderCursorAbilityTask::OnTargetDataSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(TargetDataHandle);
	}
}
