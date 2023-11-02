// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
void UAttributeMenuWidgetController::BrodcastInitialValues()
{
	//���� �Ӽ��� ������
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		// FAuraAttributeInfo���� ����Ƽ�� �±׿� ������ �±׸� ã���ϴ�.
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);

		// �±׿� ���� FAuraAttributeInfo�� ã������,
		// �ش� �±��� ���� AS (Ability System Component)�� ����Ͽ� TMap���� ���� ������ �����ͼ� FAuraAttributeInfo�� �Ҵ��մϴ�.
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);

		// FAuraAttributeInfo ������ ���� ������ ����� �� �ֵ��� ��ε�ĳ�����մϴ�.
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}
