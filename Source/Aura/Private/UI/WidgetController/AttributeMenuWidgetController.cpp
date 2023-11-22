// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include <PlayerState/AuraPlayerState.h>
#include <AbilitySystem/AuraAbilitySystemComponent.h>
void UAttributeMenuWidgetController::BrodcastInitialValues()
{
	//오라석 속성을 가져옴

	check(AttributeInfo);
	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	AttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttribtuePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData& Data)
			{	
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points) 
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpgradeAttribute(AttributeTag);

}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	// FAuraAttributeInfo에서 네이티브 태그와 동일한 태그를 찾습니다.
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);

	// 태그에 대한 FAuraAttributeInfo를 찾았으면,
	// 해당 태그의 값을 AS (Ability System Component)을 사용하여 TMap에서 숫자 값으로 가져와서 FAuraAttributeInfo에 할당합니다.
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);

	// FAuraAttributeInfo 정보를 게임 내에서 사용할 수 있도록 브로드캐스팅합니다.
	AttributeInfoDelegate.Broadcast(Info);
}

