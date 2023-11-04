// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouser.h"

UTargetDataUnderMouser* UTargetDataUnderMouser::CreateTargetDataUnderMouser(UGameplayAbility* OwingAbility)
{
	UTargetDataUnderMouser* MyObj = NewAbilityTask<UTargetDataUnderMouser>(OwingAbility);
	return MyObj;
}

void UTargetDataUnderMouser::Activate()
{
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	ValidData.Broadcast(CursorHit.Location);
}
