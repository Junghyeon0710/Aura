#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

//정적 변수의 경우 CPP 파일로 이동하여 유형을 명시적으로 선언해야 합니다.
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	//네이티브 게임플레이 태그를 추가합니다.

	/**
	* 주요스텟
	*/
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), 
		FString("물리 데미지를 증가"));

	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("마법 데미지를 증가"));

	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("방어구 관통력 증가"));

	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("체력 회복량 증가"));

	/**
	* 보조스탯
	*/
	GameplayTags.Attributes_secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("데미지를 줄임"));

	GameplayTags.Attributes_secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("방어구 관통력 증가"));

	GameplayTags.Attributes_secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.secondary.BlockChance"),
		FString("피해량 절반 감소 찬스"));

	GameplayTags.Attributes_secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.secondary.CriticalHitChance"),
		FString("크리티컬 확률"));

	GameplayTags.Attributes_secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamag"),
		FString("크리티컬 데미지증가"));

	GameplayTags.Attributes_secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.secondary.CriticalHitResistance"),
		FString("치명타 저항"));

	GameplayTags.Attributes_secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.secondary.HealthRegeneration"),
		FString("체력회복량 증가"));

	GameplayTags.Attributes_secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.secondary.ManaRegeneration"),
		FString("마나 회복량 증가"));

	GameplayTags.Attributes_secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("체력 증가"));

	GameplayTags.Attributes_secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.secondary.MaxMana"),
		FString("최대마나"));
	

	/**
	* 인풋 
	*/

	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("왼쪽마우스 클릭"));

	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("오른쪽마우스 클릭"));

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("키보드 1키"));

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("키보드 2키"));

	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("키보드 3"));

	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("키보드 4"));
}
