// Copyright Jordan Jackson


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilityTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistancePhysical);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceArcane);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceFire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceLightning);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistancePhysical, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistanceArcane, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistanceFire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistanceLightning, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;

	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistancePhysicalDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceArcaneDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceFireDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceLightningDef);

	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = ExecutionParams.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	// Physical
	float PhysicalDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Damage.Physical"));
	float PhysicalResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistancePhysicalDef, EvaluationParams, PhysicalResistance);
	PhysicalResistance = FMath::Clamp(PhysicalResistance, 0.0f, 100.0f);
	PhysicalDamage *= 1.0f - (0.01f * PhysicalResistance);

	// Arcane
	float ArcaneDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Damage.Arcane"));
	float ArcaneResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceArcaneDef, EvaluationParams, ArcaneResistance);
	ArcaneResistance = FMath::Clamp(ArcaneResistance, 0.0f, 100.0f);
	ArcaneDamage *= 1.0f - (0.01f * ArcaneResistance);

	// Fire
	float FireDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Damage.Fire"));
	float FireResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceFireDef, EvaluationParams, FireResistance);
	FireResistance = FMath::Clamp(FireResistance, 0.0f, 100.0f);
	FireDamage *= 1.0f - (0.01f * FireResistance);

	// Lightning
	float LightningDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Damage.Lightning"));
	float LightningResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceLightningDef, EvaluationParams, LightningResistance);
	LightningResistance = FMath::Clamp(LightningResistance, 0.0f, 100.0f);
	LightningDamage *= 1.0f - (0.01f * LightningResistance);

	// Total Damage
	float Damage = PhysicalDamage + ArcaneDamage + FireDamage + LightningDamage;

	// Capture Block Chance on Target, and determine if there was a successful block
	float TargetBlockChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParams, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.0f);

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	// If Block, half damage
	const bool bBlocked = FMath::RandRange(0, 100) <= TargetBlockChance;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	Damage = bBlocked ? Damage * 0.5f : Damage;

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	// Critical
	float SourceCriticalHitChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParams, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.0f);

	float TargetCriticalHitResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParams, TargetCriticalHitResistance);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPowerLevel());

	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.0f) * CriticalHitResistanceCoefficient;

	const float EffectiveCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance - TargetCriticalHitResistance, 0.0f);

	const bool bCriticalHit = FMath::RandRange(0, 100) <= EffectiveCriticalHitChance;
	if (bCriticalHit)
	{
		float SourceCriticalHitDamage = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParams, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.0f);

		Damage += Damage * (1.0f + (SourceCriticalHitDamage * 0.01f));
	}
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPowerLevel());

	// Armor and Armor Penetration
	float TargetArmor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParams, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.0f);

	float SourceArmorPenetration = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.0f) * ArmorPenetrationCoefficient;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(SourceCombatInterface->GetPowerLevel());

	const float EffectiveArmor = FMath::Max<float>(0.0f, TargetArmor * ((100.0f - SourceArmorPenetration) / 100.0f)) * EffectiveArmorCoefficient;
	Damage *= (100.0f - EffectiveArmor) / 100.0f;

	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
