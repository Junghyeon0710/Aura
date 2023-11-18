// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * �����÷��̱��ױ� 
 * ��Ƽ�� �±� ������
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

	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag Abilitiyes_Attack;

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	TMap<FGameplayTag,FGameplayTag> DamageTypesToResistances;

	FGameplayTag Effects_HitReact;
protected:

private:
	static FAuraGameplayTags GameplayTags;
};