// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParms, FGameplayEffectCustomExecutionOutput& OutExecutionOutput)const override;
	void DeterminDebuff(const FGameplayEffectSpec& Spec,
		const FGameplayEffectCustomExecutionParameters& ExecutionParms,
		FAggregatorEvaluateParameters& EvaluationParameters, 
		const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const;
};
