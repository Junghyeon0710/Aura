// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BrodcastInitialValues() override;
	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSiganture SpellPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobesSelectdDelegate;

	//장착 버튼 누르면 호출 델리게이트
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	//장착 버튼 누르면 호출된 델리게이트 멈춤
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GlobeDeselcet();

	//장착 버튼 투름
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

private:

	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);
	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None,FAuraGameplayTags::Get().Abilities_Status_Locked };
	int32 CurrentSpellPoints = 0;
	bool bWaitingForEquipSelection = false;
};
