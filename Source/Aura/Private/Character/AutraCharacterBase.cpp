// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AutraCharacterBase.h"
#include "AbilitySystemComponent.h"

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

void AAutraCharacterBase::InitAbilityActorInfo()
{
}

void AAutraCharacterBase::InitializePrimaryAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()));

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle =  GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}


