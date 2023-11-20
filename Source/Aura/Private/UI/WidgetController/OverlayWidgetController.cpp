// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "PlayerState/AuraPlayerState.h"

void UOverlayWidgetController::BrodcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana()); 

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	/** ü�°��� �ٲ�� ȣ��Ǵ� ��������Ʈ�� ���ٷ� ���ε�*/
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) //���� �ٲﰪ�� �������
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
	);

	if (UAuraAbilitySystemComponent* AuraAsc = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		//�ɷ��� ���� �־��� (���ε��� ���ߴµ� ����� �ϸ� �ƹ��͵� �ȵǼ� �׳� ����)
		if (AuraAsc->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(AuraAsc);
		}
		else //�ɷ��� �ʰ� �־��� ���ε� �ϰ� ����� ��ٸ�
		{
			AuraAsc->AbilitiesGivenDeleate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}

		/* ���� ���� ����**/
		AuraAsc->EffectAssetTags.AddLambda(
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			AbilitiyInfoDelegate.Broadcast(Info);

		});
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate); //���ε� �Ǿ��ִ°� ����
}

void UOverlayWidgetController::OnXPChanged(int32 NewXp)const
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelupInfo;

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
