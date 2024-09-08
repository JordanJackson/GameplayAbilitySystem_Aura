// Copyright Jordan Jackson

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FAuraAttributeInfo;
class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);


UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
	//// Primary
	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnStrengthChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnIntelligenceChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnResilienceChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnVigorChanged;

	//// Secondary
	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnArmorChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnArmorPenetrationChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnCriticalHitChanceChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnCriticalHitDamageChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnCriticalHitResistanceChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnHealthRegenerationChanged;

	//UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	//FOnAttributeChangedSignature OnManaRegenerationChanged;
};
