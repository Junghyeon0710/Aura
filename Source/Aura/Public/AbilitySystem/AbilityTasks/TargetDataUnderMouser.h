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
//HidePin = "OwningAbility": �������Ʈ���� �ش� �Ű������� ����ϴ�.
//DefaultToSelf = "OwningAbility": OwningAbility��� ��ü�� �Լ��� �⺻ ������ ����մϴ�.�Ϲ������� �� �Ű������� �Լ��� ȣ���� ��ü(�������Ʈ������ Self �Ǵ� Target�̶�� ��)�� �����ϴ� �� ���˴ϴ�.
//BlueprintInternalUseOnly = "true" : �� �Լ��� �������Ʈ ���ο����� ��� �����ϸ�, �������Ʈ ����ڰ� ���� ȣ���� �� �����ϴ�.