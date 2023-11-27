// Copyright Druid Mechanics


#include "Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)
	{
		//람다는 캡쳐를 참고함 weak로 참조 카운트 안하게 해줌
		//객체 소유권은 캐릭터한테만 있음
		//참조를 유지하는 동안 삭제되면 댕글링 포인터가 될 수 있다.
		// 캡처된 변수의 수명에 의존합니다. 만약 람다 함수가 캡처한 변수의 수명이 끝난 후에 호출되면 예기치 않은 동작이 발생
		//그러므로 안전하게 Weak람다를 해줘서 참조를 안전하게 해줌
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
			{
				InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
			});
	}
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwerDeath);
	}

}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bOwnerValid = IsValid(GetOwner());
	const bool bOwnerAlive = GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());
	
	if (NewCount > 0 && bOwnerValid && bOwnerAlive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwerDeath(AActor* DeadActor)
{
	Deactivate();
}
