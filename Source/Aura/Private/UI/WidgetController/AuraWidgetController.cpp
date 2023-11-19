// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParms(const FWidgetControllerParms& WCparms)
{
	PlayerController = WCparms.PlayerController;
	PlayerState = WCparms.PlayerState;
	AbilitySystemComponent = WCparms.AbilitySystemComponent;
	AttributeSet = WCparms.AttributeSet;
}

void UAuraWidgetController::BrodcastInitialValues()
{

}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}


