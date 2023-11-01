// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_MaxMana();

	//수정자가 생성해야 하는 결과
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec)const override;

private:
	//게임플레이 이펙트가 적용되는 대상의 게임 속성을 추적하고 변경하는 데 사용됩니다.
	FGameplayEffectAttributeCaptureDefinition IntDef;
};
