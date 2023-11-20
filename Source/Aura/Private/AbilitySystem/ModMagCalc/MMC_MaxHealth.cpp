// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	//���ӿ��� "ĸó(Capture)"�� Ư�� ������ �����͸� ����ϰų� �����ϴ� �۾��� ��Ÿ���ϴ�.
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	//�迭�� �߰������ν� �ش� �Ӽ��� ������ �پ��� �κп��� ������ �� �ְ� �˴ϴ�.
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); //��� �±׸� ������
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//���� ȿ���� �򰡿� �ʿ��� ������ �����ϰ� �����ϴ� �� ���˴ϴ�. 
	//�� ����ü�� ���Ե� ������ ���� ȿ���� ��� ���ǰ� ����Ǵ����� ������ ��Ĩ�ϴ�.
	FAggregatorEvaluateParameters EvaluationParmeters;
	EvaluationParmeters.SourceTags = SourceTags;
	EvaluationParmeters.TargetTags = TargetTags;

	float Vigor = 0.f;
	// Captured Attribute Magnitude �Լ��� ����Ͽ� Vigor ���� �����ɴϴ�.
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParmeters, Vigor);
	// ���� ���� ���� �ʵ��� 0 �̻����� �����մϴ�.
	Vigor = FMath::Max<float>(Vigor, 0.f);

	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
