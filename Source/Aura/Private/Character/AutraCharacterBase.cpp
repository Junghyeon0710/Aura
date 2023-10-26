// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AutraCharacterBase.h"

AAutraCharacterBase::AAutraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAutraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAutraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


