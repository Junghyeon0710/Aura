// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"

//�Ϲ������� �� �۾��� ������ ������ ������ �� �����ϸ� �����ڿ��� �������� �ʽ��ϴ�. �����ڴ� ����� ���� ȣ��˴ϴ�.
//���� �� �븮�ڿ� ���� ���ε��� �� �ִ� ��ġ�� �ִٸ� ���� ���Դϴ�.���� ��� ���� �ɷ� ���� ������ �ʱ�ȭ�� �Ŀ� 
//ȣ���� �� �ִ� �Լ��� ����� ���� �����ϴ�.�װ��� ����� �ʱ⿡ ȣ���� �� �ִ� �����Դϴ�.
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// �ڽ�(Self)���� �����Ƽ ȿ���� ����� �� ȣ��Ǵ� ��������Ʈ�Դϴ�.
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	//GameplayTags.Attributes_secondary_Armor.ToString()
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); //���� �ִ� �ױ׸� ���� ������

	EffectAssetTags.Broadcast(TagContainer); //OverlayWidgetController�� ��ε��ɽ�Ʈ(����)
}
