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

	/** 체력값이 바뀌면 호출되는 델리게이트에 람다로 바인딩*/
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) //값이 바뀐값이 들어있음
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
		//능력이 먼저 주어짐 (바인딩을 안했는데 방송을 하면 아무것도 안되서 그냥 실행)
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else //능력이 늦게 주어짐 바인딩 하고 방송을 기다림
		{
			GetAuraASC()->AbilitiesGivenDeleate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		/* 포션 먹후 직후**/
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

	checkf(LevelUpInfo, TEXT("플레이어 스테이트에서 설정해주세여"));

	const int32 Level = LevelUpInfo->FindLevelForXp(NewXp);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		//1레벌 요구사항 300
		//2레벨 요구사항 900이라 가정

		//현재 레벨 경험치 요구사항 총량
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelupRequirement;
		//전 레벨 경험치 요구사항 총량
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelupRequirement;
		//레벨업에 필요한 경험치 총량
		const int32 DeltaLevelUpRequirement =  LevelUpRequirement - PreviousLevelUpRequirement;
		//내 경험치에서 전 레벨 경험치 요구사항을 뺌(그러면 그레벨에서 부터 얼마나 필요한지 알 수 있음)
		//2레벨이면 내가 갖고있는 경험치 500 - 전레벨 경험치 요구사항(300) = 200
		const int32 XpForThisLevel = NewXp - PreviousLevelUpRequirement;
		//내가 레벨에서 갖고있는 경험치에서 레벨업에 필요한 경험치 총량을 나눔
		//200/600 = 0.333퍼
		const float XpBarPercent = static_cast<float>(XpForThisLevel) / static_cast<float>(DeltaLevelUpRequirement);

		//현재 경험치 퍼센트 방송
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