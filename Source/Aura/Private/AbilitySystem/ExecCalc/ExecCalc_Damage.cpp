// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Actor/AuraAbilityTypes.h"


struct AuraDamageStatics
{
	//ĸó ��ũ��
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaputreDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaputreDefs.Add(Tags.Attributes_secondary_BlockChance, BlockChanceDef);
		TagsToCaputreDefs.Add(Tags.Attributes_secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaputreDefs.Add(Tags.Attributes_secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaputreDefs.Add(Tags.Attributes_secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaputreDefs.Add(Tags.Attributes_secondary_CriticalHitResistance, CriticalHitResistanceDef);

		TagsToCaputreDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaputreDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaputreDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaputreDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);

	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;

	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);

}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParms, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParms.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParms.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParms.GetOwningSpec();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	//Magnitudeȣ���ؼ� ������ �Ա�
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		checkf(AuraDamageStatics().TagsToCaputreDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't Cotain Tag"));
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaputreDefs[ResistanceTag];
		
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);

		float Resistance = 0.f;
		ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue *= (100.f - Resistance) / 100.f;

		Damage += DamageTypeValue;
	}

	//Ÿ�� ������� ĸó, �������� ����� �־���
	//������ ������ �������� ����
	float TargetBlockChance = 0.f;
	//������ �Ű������� �־����� ĸó�� �Ӽ��� ũ�⸦ ����Ϸ��� �õ��մϴ�. 
	ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);


	if (bBlocked)
	{
		Damage = Damage / 2.f;
	}

	//�� ������� ��� ������ ������
	float TargetArmor = 0.f;
	ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	//�ɸ��� Ŭ���� ���� ������ ������ ������
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	//�������ִ� Ŀ�갪�� ������
	const FRealCurve* ArmorPentrationCurve =  CharacterClassInfo->DamageCalulationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	//�����̶� ���� Ŀ�갪�� ������
	const float ArmorPenetrationCoefficient = ArmorPentrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	//�� ����� ����
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalulationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	/** ġ��Ÿ */
	float SourceCriticalHitChance = 0.f;
	ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParms.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitDamage, 0.f);
	
	//������ ũ��Ƽ�� ���׷� ����
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalulationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	//ġ��Ÿ ������ ġ��Ÿ Ȯ���� ���� ������ŭ ���� ��Ŵ
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	//ġ��Ÿ�� ����Ȯ��
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	//������ ������ �ι� + ũ��Ƽ�� ������
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
	
	
	//�������� �ִ�.
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
