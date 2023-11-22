// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include <Interaction/PlayerInterface.h>
#include "AbilitySystemBlueprintLibrary.h"

//일반적으로 이 작업은 게임이 실제로 시작할 때 수행하며 생성자에서 수행하지 않습니다. 생성자는 상당히 일찍 호출됩니다.
//따라서 이 대리자에 일찍 바인딩할 수 있는 위치가 있다면 좋을 것입니다.예를 들어 게임 능력 액터 정보를 초기화한 후에 
//호출할 수 있는 함수를 만드는 것이 좋습니다.그것은 상당히 초기에 호출할 수 있는 지점입니다.
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 자신(Self)에게 어빌리티 효과가 적용될 때 호출되는 델리게이트입니다.
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	//GameplayTags.Attributes_secondary_Armor.ToString()
}

//CharacterBase에서 호출
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// FGameplayAbilitySpec 구조체로 초기화
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// 어빌리티가 UAuraGameplayAbility의 하위 클래스인 경우
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//동적 태그에 추가
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			
			GiveAbility(AbilitySpec); //능력부여
		}
		//GiveAbilityAndActivateOnce(AbilitySpec); //능력 한번만 부여

	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDeleate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		// FGameplayAbilitySpec 구조체로 초기화
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		GiveAbilityAndActivateOnce(AbilitySpec); //능력부여
	}
}

//AuraPlayerController에서 호출
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//활성화된 어빌리티를 다 가져옴
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//동적으로 할당된 능력 태그에 InputTag가 정확히 일치하는지 확인합니다.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 능력이 활성화되었을 때 호출되는 함수로 해당 능력에 대한 입력이 눌린 경우 실행됩니다. 
			AbilitySpecInputPressed(AbilitySpec);
			//능력이 활성화가 안되있으면
			if (!AbilitySpec.IsActive())
			{
				//지정된 능력을 활성화하려고 시도하는 함
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//활성화된 어빌리티를 다 가져옴
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//동적으로 할당된 능력 태그에 InputTag가 정확히 일치하는지 확인합니다.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//놓았을 때 호출되는 함수입니다.
			//능력이 비활성화됨
			AbilitySpecInputReleased(AbilitySpec);
		}
	}

}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this); //잠금

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))//바인딩 되어있으면 실행
		{
			//바인딩 되어있지 않으면 에러 출력 
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
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) //태그를 반환
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//동적으로 추가해서 동적으로 찾아야함
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
	//동적으로 추가해서 동적으로 찾아야함
	for (FGameplayTag StausTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StausTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StausTag;
		}
	}
	return FGameplayTag();
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

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDeleate.Broadcast();
	}

}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); //갖고 있는 테그를 전부 가져옴

	EffectAssetTags.Broadcast(TagContainer); //OverlayWidgetController에 브로드케스트(전달)
}
