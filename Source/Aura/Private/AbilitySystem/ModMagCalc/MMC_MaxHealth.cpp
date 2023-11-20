// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	//게임에서 "캡처(Capture)"란 특정 정보나 데이터를 기록하거나 저장하는 작업을 나타냅니다.
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	//배열에 추가함으로써 해당 속성이 게임의 다양한 부분에서 추적될 수 있게 됩니다.
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); //모든 태그를 가져옴
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//게임 효과의 평가에 필요한 정보를 보유하고 전달하는 데 사용됩니다. 
	//이 구조체에 포함된 정보는 게임 효과가 어떻게 계산되고 적용되는지에 영향을 미칩니다.
	FAggregatorEvaluateParameters EvaluationParmeters;
	EvaluationParmeters.SourceTags = SourceTags;
	EvaluationParmeters.TargetTags = TargetTags;

	float Vigor = 0.f;
	// Captured Attribute Magnitude 함수를 사용하여 Vigor 값을 가져옵니다.
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParmeters, Vigor);
	// 음수 값이 되지 않도록 0 이상으로 보정합니다.
	Vigor = FMath::Max<float>(Vigor, 0.f);

	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
