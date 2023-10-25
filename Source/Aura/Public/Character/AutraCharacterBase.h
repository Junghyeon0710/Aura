// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AutraCharacterBase.generated.h"

UCLASS(Abstract) // 클래스 레벨 소환 방지
class AURA_API AAutraCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAutraCharacterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<USkeletalMeshComponent> Weapon;

};
