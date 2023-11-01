// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAuraAssetManager& Get();

protected:
	//언리얼 엔진 킬때 [Config]폴더 -> DefaultEngine.ini에서 지정해줘야함
	//[/Script/Engine.Engine]찾아서 
	//AssetManagerClassName=/Script/Aura(게임이름).AuraAssetManager(이 클래스 이름)
	virtual void StartInitialLoading() override;
};
