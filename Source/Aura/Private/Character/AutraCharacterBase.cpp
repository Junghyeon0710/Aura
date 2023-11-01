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

void AAutraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAutraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	//MaxHealth과 MaxMana를 따라가기때문에 먼저 해줘야 가능
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}


