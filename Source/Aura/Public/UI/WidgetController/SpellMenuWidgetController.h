// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BrodcastInitialValues() override;
	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSiganture SpellPointsChanged;


};
