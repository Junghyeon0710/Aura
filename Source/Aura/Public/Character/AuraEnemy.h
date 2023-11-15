// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AutraCharacterBase.h"
#include "../Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "../AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"


class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAutraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	/**적 인테페이스 */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/**적 인테페이스 */

	/** Combat Interface*/
	virtual int32 GetPlayerLevel() override;
	/** end Combat Interface*/
	virtual void Die() override;
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSiganture OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSiganture OnMaxHealthChanged;

	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LifeSpan = 5.f;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes()const override;
	/** 캐릭터 레벨 (Target)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "Ai")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
	
};
