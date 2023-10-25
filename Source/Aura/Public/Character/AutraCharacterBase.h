// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AutraCharacterBase.generated.h"

UCLASS(Abstract) // Ŭ���� ���� ��ȯ ����
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
