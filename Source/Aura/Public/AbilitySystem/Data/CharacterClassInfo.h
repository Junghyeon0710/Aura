// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};
/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Commont Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Commont Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Commont Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Commont Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalulationCoefficients;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
