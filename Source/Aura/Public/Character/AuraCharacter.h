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
	virtual void PossessedBy(AController* NewController)override; //이 Pawn이 빙의되었을 때 호출됩니다. 서버에서만(또는 독립형으로) 호출됩니다.
	virtual void OnRep_PlayerState() override; //임의 플레이어 상태(PlayerState)가 변경되면 해당 변경 사항을 모든 클라이언트에 동기화하거나 게임 상태에 반영하는 데 사용할 수 있습니다.

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

	/** end Player Interface*/
private:
	virtual void InitAbilityActorInfo() override;

};
