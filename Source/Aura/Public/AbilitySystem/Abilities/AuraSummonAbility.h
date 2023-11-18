// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocation();

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	TArray<TSubclassOf<APawn>> MinonClasses;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	float SpawnSpread = 90.f;
};
