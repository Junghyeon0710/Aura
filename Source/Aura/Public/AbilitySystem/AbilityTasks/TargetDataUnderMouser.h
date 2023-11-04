// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouser.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FVector&, Data);

/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouser : public UAbilityTask
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,Category="Ablility|Tasks",meta = (DisplayName = "TargetDataUnderMouse",HidePin = "OwingAbility", DefaultToself = "OwingAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouser* CreateTargetDataUnderMouser(UGameplayAbility* OwingAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

private:
	virtual void Activate() override;
};
//HidePin = "OwningAbility": 블루프린트에서 해당 매개변수를 감춥니다.
//DefaultToSelf = "OwningAbility": OwningAbility라는 객체를 함수의 기본 값으로 사용합니다.일반적으로 이 매개변수는 함수를 호출한 객체(블루프린트에서는 Self 또는 Target이라고도 함)를 지정하는 데 사용됩니다.
//BlueprintInternalUseOnly = "true" : 이 함수는 블루프린트 내부에서만 사용 가능하며, 블루프린트 사용자가 직접 호출할 수 없습니다.