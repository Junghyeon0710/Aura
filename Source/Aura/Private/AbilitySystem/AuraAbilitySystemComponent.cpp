// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

//�Ϲ������� �� �۾��� ������ ������ ������ �� �����ϸ� �����ڿ��� �������� �ʽ��ϴ�. �����ڴ� ����� ���� ȣ��˴ϴ�.
//���� �� �븮�ڿ� ���� ���ε��� �� �ִ� ��ġ�� �ִٸ� ���� ���Դϴ�.���� ��� ���� �ɷ� ���� ������ �ʱ�ȭ�� �Ŀ� 
//ȣ���� �� �ִ� �Լ��� ����� ���� �����ϴ�.�װ��� ����� �ʱ⿡ ȣ���� �� �ִ� �����Դϴ�.
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// �ڽ�(Self)���� �����Ƽ ȿ���� ����� �� ȣ��Ǵ� ��������Ʈ�Դϴ�.
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	//GameplayTags.Attributes_secondary_Armor.ToString()
}

//CharacterBase���� ȣ��
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// FGameplayAbilitySpec ����ü�� �ʱ�ȭ
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// �����Ƽ�� UAuraGameplayAbility�� ���� Ŭ������ ���
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//���� �±׿� �߰�
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec); //�ɷºο�
		}
		//GiveAbilityAndActivateOnce(AbilitySpec); //�ɷ� �ѹ��� �ο�

	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDeleate.Broadcast(this);
}

//AuraPlayerController���� ȣ��
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//Ȱ��ȭ�� �����Ƽ�� �� ������
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//�������� �Ҵ�� �ɷ� �±׿� InputTag�� ��Ȯ�� ��ġ�ϴ��� Ȯ���մϴ�.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// �ɷ��� Ȱ��ȭ�Ǿ��� �� ȣ��Ǵ� �Լ��� �ش� �ɷ¿� ���� �Է��� ���� ��� ����˴ϴ�. 
			AbilitySpecInputPressed(AbilitySpec);
			//�ɷ��� Ȱ��ȭ�� �ȵ�������
			if (!AbilitySpec.IsActive())
			{
				//������ �ɷ��� Ȱ��ȭ�Ϸ��� �õ��ϴ� ��
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//Ȱ��ȭ�� �����Ƽ�� �� ������
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//�������� �Ҵ�� �ɷ� �±׿� InputTag�� ��Ȯ�� ��ġ�ϴ��� Ȯ���մϴ�.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//������ �� ȣ��Ǵ� �Լ��Դϴ�.
			//�ɷ��� ��Ȱ��ȭ��
			AbilitySpecInputReleased(AbilitySpec);
		}
	}

}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this); //���

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))//���ε� �Ǿ������� ����
		{
			//���ε� �Ǿ����� ������ ���� ��� 
			UE_LOG(LogTemp, Error, TEXT("Failed to Execute Delegate"));
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) //�±׸� ��ȯ
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//�������� �߰��ؼ� �������� ã�ƾ���
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDeleate.Broadcast(this);
	}

}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); //���� �ִ� �ױ׸� ���� ������

	EffectAssetTags.Broadcast(TagContainer); //OverlayWidgetController�� ��ε��ɽ�Ʈ(����)
}
