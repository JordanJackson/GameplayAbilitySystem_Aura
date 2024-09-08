// Copyright Jordan Jackson

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MaxVitalModMagnitudeCalculation.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMaxVitalModMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMaxVitalModMagnitudeCalculation();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float BaseValue;

	UPROPERTY(EditDefaultsOnly)
	float PowerLevelCoefficient;

	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute PrimaryAttribute;

	UPROPERTY(EditDefaultsOnly)
	float PrimaryAttributeCoefficient;
};
