// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include <Interaction/PlayerInterface.h>
#include "AbilitySystemBlueprintLibrary.h"
#include <AbilitySystem/AuraAbilitySystemLibrary.h>
#include "AbilitySystem/Data/AbilityInfo.h"

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
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			
			GiveAbility(AbilitySpec); //�ɷºο�
		}
		//GiveAbilityAndActivateOnce(AbilitySpec); //�ɷ� �ѹ��� �ο�

	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDeleate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		// FGameplayAbilitySpec ����ü�� �ʱ�ȭ
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		GiveAbilityAndActivateOnce(AbilitySpec); //�ɷºο�
	}
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

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//�������� �߰��ؼ� �������� ã�ƾ���
	for (FGameplayTag StausTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StausTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StausTag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec; //������ �տ� ���̸� �ּҰ���
			}
		}
	}

	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());

	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInfomation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level < Info.LevelRequirement) //������ �䱸���׺��� ������
		{
			continue; // �ɷ��� Ȱ��ȭ �ϸ� �ȵ�
		}
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr) //�ɷ��� ������
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			//�� �Լ��� ȣ���Ͽ� ����� �ɷ� ������ ǥ���ϸ� ������ �̸� �����ϰ� Ŭ���̾�Ʈ���� �ش� ���� ������ �����մϴ�.
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible,1);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		//����Ʈ 1���
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		//Ȱ��ȭ �� �� �ִ� �������̶��
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			//�±� ����
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			//�±� �߰�
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_UnLocked);
			//���¸� ��� Ǯ������ �ٲ�
			Status = GameplayTags.Abilities_Status_UnLocked;
		}//������ �����̰ų� ��� Ǯ���ִ� ���¸� ������
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_UnLocked))
		{
			AbilitySpec->Level += 1;
		}
		//Ŭ���̾�Ʈ�� �˷���
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		//�� �Լ��� ȣ���Ͽ� ����� �ɷ� ������ ǥ���ϸ� ������ �̸� �����ϰ� Ŭ���̾�Ʈ���� �ش� ���� ������ �����մϴ�.
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDeleate.Broadcast();
	}

}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); //���� �ִ� �ױ׸� ���� ������

	EffectAssetTags.Broadcast(TagContainer); //OverlayWidgetController�� ��ε��ɽ�Ʈ(����)
}
