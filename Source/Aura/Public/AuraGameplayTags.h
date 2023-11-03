// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 게임플레이그테그 
 * 나티브 태그 만들기용
 */


struct  FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	FGameplayTag Attributes_secondary_Armor;
	FGameplayTag Attributes_secondary_ArmorPenetration;
	FGameplayTag Attributes_secondary_BlockChance;
	FGameplayTag Attributes_secondary_CriticalHitChance;
	FGameplayTag Attributes_secondary_CriticalHitDamage;
	FGameplayTag Attributes_secondary_CriticalHitResistance;
	FGameplayTag Attributes_secondary_HealthRegeneration;
	FGameplayTag Attributes_secondary_ManaRegeneration;
	FGameplayTag Attributes_secondary_MaxHealth;
	FGameplayTag Attributes_secondary_MaxMana;

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

protected:

private:
	static FAuraGameplayTags GameplayTags;
};