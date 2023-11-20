// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AutraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAutraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController)override; //�� Pawn�� ���ǵǾ��� �� ȣ��˴ϴ�. ����������(�Ǵ� ����������) ȣ��˴ϴ�.
	virtual void OnRep_PlayerState() override; //���� �÷��̾� ����(PlayerState)�� ����Ǹ� �ش� ���� ������ ��� Ŭ���̾�Ʈ�� ����ȭ�ϰų� ���� ���¿� �ݿ��ϴ� �� ����� �� �ֽ��ϴ�.

	/** Combat Interface*/
	virtual int32 GetPlayerLevel() override;
	/** end Combat Interface*/

	/** Player Interface*/
	virtual void AddToXp_Implementation(int32 InXp) override;
	/** end Player Interface*/
private:
	virtual void InitAbilityActorInfo() override;

};
