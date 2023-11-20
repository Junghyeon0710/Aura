// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	//���ӿ��� "ĸó(Capture)"�� Ư�� ������ �����͸� ����ϰų� �����ϴ� �۾��� ��Ÿ���ϴ�.
	IntDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false;

	//�迭�� �߰������ν� �ش� �Ӽ��� ������ �پ��� �κп��� ������ �� �ְ� �˴ϴ�.
	RelevantAttributesToCapture.Add(IntDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); //��� �±׸� ������
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//���� ȿ���� �򰡿� �ʿ��� ������ �����ϰ� �����ϴ� �� ���˴ϴ�. 
	//�� ����ü�� ���Ե� ������ ���� ȿ���� ��� ���ǰ� ����Ǵ����� ������ ��Ĩ�ϴ�.
	FAggregatorEvaluateParameters EvaluationParmeters;
	EvaluationParmeters.SourceTags = SourceTags;
	EvaluationParmeters.TargetTags = TargetTags;

	float Int = 0.f;
	// Captured Attribute Magnitude �Լ��� ����Ͽ� Int ���� �����ɴϴ�.
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluationParmeters, Int);
	// ���� ���� ���� �ʵ��� 0 �̻����� �����մϴ�.
	Int = FMath::Max<float>(Int, 0.f);

	
	// ApplyGameplayEffect���ٶ� AddSourceObject�� ������� �� �׷� ��
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	//(�⺻�� + ��Ʈ��*.25 + ������ *10 )
	return 50.f + 2.5f * Int + 15.f * PlayerLevel;
}
