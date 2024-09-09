// Copyright Jordan Jackson

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderCursorAbilityTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCursorTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

UCLASS()
class AURA_API UTargetDataUnderCursorAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderCursor", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UTargetDataUnderCursorAbilityTask* CreateTargetDataUnderCursor(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FCursorTargetDataSignature ValidData;

private:
	virtual void Activate() override;

	void SendCursorTargetData();

	UFUNCTION()
	void OnTargetDataSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag);
};
