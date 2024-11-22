// Copyright Jordan Jackson

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighlightInterface, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController);

	// Highlight Interface
	virtual void Highlight() override;
	virtual void ClearHighlight() override;

	// Combat Interface
	virtual int32 GetPowerLevel() const override;
	virtual void Die() override;

	// Enemy Interface
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;

	virtual void MulticastHandleDeath_Implementation() override;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReactActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed= 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifespanAfterDeath = 5.0f;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> StatusBarWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
