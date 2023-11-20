// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	//게임에서 "캡처(Capture)"란 특정 정보나 데이터를 기록하거나 저장하는 작업을 나타냅니다.
	IntDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false;

	//배열에 추가함으로써 해당 속성이 게임의 다양한 부분에서 추적될 수 있게 됩니다.
	RelevantAttributesToCapture.Add(IntDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); //모든 태그를 가져옴
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//게임 효과의 평가에 필요한 정보를 보유하고 전달하는 데 사용됩니다. 
	//이 구조체에 포함된 정보는 게임 효과가 어떻게 계산되고 적용되는지에 영향을 미칩니다.
	FAggregatorEvaluateParameters EvaluationParmeters;
	EvaluationParmeters.SourceTags = SourceTags;
	EvaluationParmeters.TargetTags = TargetTags;

	float Int = 0.f;
	// Captured Attribute Magnitude 함수를 사용하여 Int 값을 가져옵니다.
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluationParmeters, Int);
	// 음수 값이 되지 않도록 0 이상으로 보정합니다.
	Int = FMath::Max<float>(Int, 0.f);

	
	// ApplyGameplayEffect해줄때 AddSourceObject를 해줘야함 안 그럼 널
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	//(기본값 + 인트당*.25 + 레벨당 *10 )
	return 50.f + 2.5f * Int + 15.f * PlayerLevel;
}
