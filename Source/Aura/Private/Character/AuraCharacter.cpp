// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true; //true인 경우 이동이 평면으로 제한됩니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // true이고 평면 제약 조건이 활성화된 경우 업데이트된 구성 요소가 처음 연결될 때 평면에 스냅됩니다.

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

