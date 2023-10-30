// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AutraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAutraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController)override; //�� Pawn�� ���ǵǾ��� �� ȣ��˴ϴ�. ����������(�Ǵ� ����������) ȣ��˴ϴ�.
	virtual void OnRep_PlayerState() override; //���� �÷��̾� ����(PlayerState)�� ����Ǹ� �ش� ���� ������ ��� Ŭ���̾�Ʈ�� ����ȭ�ϰų� ���� ���¿� �ݿ��ϴ� �� ����� �� �ֽ��ϴ�.
private:
	virtual void InitAbilityActorInfo() override;

};
