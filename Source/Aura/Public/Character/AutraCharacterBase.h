// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "../Interaction/CombatInterface.h"
#include "../AbilitySystem/Data/CharacterClassInfo.h"
#include "AutraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UAnimMontage;
class UNiagaraSystem;

UCLASS(Abstract) // 클래스 레벨 소환 방지
class AURA_API AAutraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	AAutraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }


	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	/** Combat Interface*/
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontaged_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage getTaggeMontageByTag_Implementation(const FGameplayTag& MontageTag);
	virtual int32 GetMinionCount_Implementation();
	virtual void IncreamentMinionCount_Implementation(int32 Amount);
	virtual ECharacterClass GetCharacterClass_Implementation();
	virtual FOnASCRegistered GetOnASCRegisteredDelegate() override;
	virtual FOnDeath GetOnDeathDelegate() override;
	/** end Combat Interface*/

	FOnASCRegistered OnASCRegistered;
	FOnDeath OnDeath;

	UPROPERTY(EditAnywhere,Category = "Combat")
	TArray<FTaggedMontage> AttackMontage;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Combat)
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightSocketName;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName TailSocketName;

	bool bDead = false;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level)const;
	virtual void InitializeDefaultAttributes()const;

	void AddCharacterAbilities();

	/** 녹는듯한 이펙트*/

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* DeathSound;

	/*미니언*/

	int32 MinionCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UDebuffNiagaraComponent> BurnDebuffComponent;
private:
	UPROPERTY(EditAnywhere, Category = Abilities)
	TArray <TSubclassOf<UGameplayAbility>> StartupAbilites;

	UPROPERTY(EditAnywhere, Category = Abilities)
	TArray <TSubclassOf<UGameplayAbility>> StartupPassiveAbilites;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactMontage;
};
