// Copyright Jordan Jackson


#include "AbilitySystem/ModMagCalc/MaxVitalModMagnitudeCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMaxVitalModMagnitudeCalculation::UMaxVitalModMagnitudeCalculation()
{

}

float UMaxVitalModMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	const FGameplayEffectAttributeCaptureDefinition* PrimaryAttributeDef = RelevantAttributesToCapture.FindByPredicate(
	[&](FGameplayEffectAttributeCaptureDefinition Def)
	{
		return Def.AttributeToCapture == PrimaryAttribute;
	});

	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;

	float PrimaryAttributeValue = 0.0f;
	if (PrimaryAttributeDef)
	{
		GetCapturedAttributeMagnitude(*PrimaryAttributeDef, Spec, EvaluationParams, PrimaryAttributeValue);
		PrimaryAttributeValue = FMath::Max<float>(PrimaryAttributeValue, 0.0f);
	}

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PowerLevel = CombatInterface->GetPowerLevel();

	return BaseValue + (PrimaryAttributeValue * PrimaryAttributeCoefficient) + (PowerLevel * PowerLevelCoefficient);
}