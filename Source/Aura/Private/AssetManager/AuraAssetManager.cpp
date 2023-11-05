// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetManager/AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	// 대상 데이터 사용하는데 필요
	//어빌리티 시스템이 올바르게 동작하기 위해 필요한 데이터를 로드하고 설정합니다
	UAbilitySystemGlobals::Get().InitGlobalData();
}
