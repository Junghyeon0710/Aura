// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AutraCharacterBase.h"
#include "../Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAutraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/**적 인테페이스 */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/**적 인테페이스 */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
};
