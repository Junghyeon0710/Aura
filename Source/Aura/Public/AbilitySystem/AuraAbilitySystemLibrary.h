// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure,Category="AuraAbilitySystemLibrary|WidgetController")
	static class UOverlayWidgetController* GetOverlayController(const UObject* WorldContextObjec);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static class UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
};
