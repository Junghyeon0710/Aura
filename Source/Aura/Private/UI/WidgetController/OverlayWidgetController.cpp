// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "PlayerState/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void UOverlayWidgetController::BrodcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	/** ü�°��� �ٲ�� ȣ��Ǵ� ��������Ʈ�� ���ٷ� ���ε�*/
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) //���� �ٲﰪ�� �������
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
	);

	if (GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		//�ɷ��� ���� �־��� (���ε��� ���ߴµ� ����� �ϸ� �ƹ��͵� �ȵǼ� �׳� ����)
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else //�ɷ��� �ʰ� �־��� ���ε� �ϰ� ����� ��ٸ�
		{
			GetAuraASC()->AbilitiesGivenDeleate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		/* ���� ���� ����**/
		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXp)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelupInfo;

	checkf(LevelUpInfo, TEXT("�÷��̾� ������Ʈ���� �������ּ���"));

	const int32 Level = LevelUpInfo->FindLevelForXp(NewXp);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		//1���� �䱸���� 300
		//2���� �䱸���� 900�̶� ����

		//���� ���� ����ġ �䱸���� �ѷ�
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelupRequirement;
		//�� ���� ����ġ �䱸���� �ѷ�
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelupRequirement;
		//�������� �ʿ��� ����ġ �ѷ�
		const int32 DeltaLevelUpRequirement =  LevelUpRequirement - PreviousLevelUpRequirement;
		//�� ����ġ���� �� ���� ����ġ �䱸������ ��(�׷��� �׷������� ���� �󸶳� �ʿ����� �� �� ����)
		//2�����̸� ���� �����ִ� ����ġ 500 - ������ ����ġ �䱸����(300) = 200
		const int32 XpForThisLevel = NewXp - PreviousLevelUpRequirement;
		//���� �������� �����ִ� ����ġ���� �������� �ʿ��� ����ġ �ѷ��� ����
		//200/600 = 0.333��
		const float XpBarPercent = static_cast<float>(XpForThisLevel) / static_cast<float>(DeltaLevelUpRequirement);

		//���� ����ġ �ۼ�Ʈ ���
		OnXpPercentChangedChanged.Broadcast(XpBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_UnLocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell
	AbilitiyInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilitiyInfoDelegate.Broadcast(Info);
}