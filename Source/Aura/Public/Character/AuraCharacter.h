// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AutraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"


class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;
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
	virtual int32 GetPlayerLevel_Implementation() override;
	/** end Combat Interface*/

	/** Player Interface*/
	virtual void AddToXp_Implementation(int32 InXp) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXp_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP)const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel)override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints)override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints)override;
	virtual int32 GetAttributePoints_Implementation() const;
	virtual int32 GetSpellPoints_Implementation() const;


	/** end Player Interface*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void OnRep_Stunned() override;

	virtual void OnRep_Burned()override;
private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast , Reliable)
	void Multicast_LevelUpParticles() const;
};
