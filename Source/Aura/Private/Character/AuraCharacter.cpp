// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true; //true�� ��� �̵��� ������� ���ѵ˴ϴ�.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // true�̰� ��� ���� ������ Ȱ��ȭ�� ��� ������Ʈ�� ���� ��Ұ� ó�� ����� �� ��鿡 �����˴ϴ�.

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

