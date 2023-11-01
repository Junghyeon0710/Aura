// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"

//일반적으로 이 작업은 게임이 실제로 시작할 때 수행하며 생성자에서 수행하지 않습니다. 생성자는 상당히 일찍 호출됩니다.
//따라서 이 대리자에 일찍 바인딩할 수 있는 위치가 있다면 좋을 것입니다.예를 들어 게임 능력 액터 정보를 초기화한 후에 
//호출할 수 있는 함수를 만드는 것이 좋습니다.그것은 상당히 초기에 호출할 수 있는 지점입니다.
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 자신(Self)에게 어빌리티 효과가 적용될 때 호출되는 델리게이트입니다.
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	//GameplayTags.Attributes_secondary_Armor.ToString()
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); //갖고 있는 테그를 전부 가져옴

	EffectAssetTags.Broadcast(TagContainer); //OverlayWidgetController에 브로드케스트(전달)
}
