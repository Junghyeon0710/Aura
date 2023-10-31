// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// 특정 타겟 액터에 게임플레이 이펙트를 적용합니다.
// 이 함수는 특정 타겟 액터에 게임플레이 이펙트를 추가하여 그들의 능력을 개선하거나 변경하는 데 사용됩니다.
// 매개변수:
// - TargetActor: 게임플레이 이펙트가 적용될 액터입니다.
// - GameplayEffectClass: 적용할 게임플레이 이펙트의 클래스입니다.
void AAuraEffectActor::ApplayEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    // 타겟 액터의 어빌리티 시스템 컴포넌트를 가져옵니다.
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    // 타겟 액터에 어빌리티 시스템 컴포넌트가 없다면 함수를 종료합니다.
    if (TargetASC == nullptr)
    {
        return;
    }

    // GameplayEffectClass가 유효한지 확인합니다.
    check(GameplayEffectClass);

    // 게임플레이 이펙트에 대한 컨텍스트를 생성합니다.
    FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();

    // 이펙트의 소스로 현재 액터(this)를 추가합니다.
    EffectContextHandle.AddSourceObject(this);

    // 제공된 클래스와 Context를 기반으로 GameplayEffectSpec을 생성합니다.
    const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);

    // 생성된 GameplayEffectSpec을 대상 액터의 Ability System Component에 적용합니다.
    const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

    const bool bIninfinte = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
   
    if (bIninfinte && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
        ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
    }
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplayOnOverlap)
    {
        ApplayEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplayOnOverlap)
    {
        ApplayEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplayOnOverlap)
    {
        ApplayEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
}

void AAuraEffectActor::EndOverlap(AActor* TargetActor)
{
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplayOnEndOverlap)
    {
        ApplayEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplayOnEndOverlap)
    {
        ApplayEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplayOnEndOverlap)
    {
        ApplayEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
    if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (!IsValid(TargetASC)) return;
        TArray<FActiveGameplayEffectHandle> HandlesToRemvoe;

        for (TTuple<FActiveGameplayEffectHandle,UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
        {
            if (TargetASC == HandlePair.Value)
            {
                TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
                HandlesToRemvoe.Add(HandlePair.Key);
            }
        }
        for (auto& Handle : HandlesToRemvoe)
        {
            ActiveEffectHandles.FindAndRemoveChecked(Handle);
        }
    }
}


