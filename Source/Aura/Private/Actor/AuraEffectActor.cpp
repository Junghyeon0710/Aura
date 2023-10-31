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

// Ư�� Ÿ�� ���Ϳ� �����÷��� ����Ʈ�� �����մϴ�.
// �� �Լ��� Ư�� Ÿ�� ���Ϳ� �����÷��� ����Ʈ�� �߰��Ͽ� �׵��� �ɷ��� �����ϰų� �����ϴ� �� ���˴ϴ�.
// �Ű�����:
// - TargetActor: �����÷��� ����Ʈ�� ����� �����Դϴ�.
// - GameplayEffectClass: ������ �����÷��� ����Ʈ�� Ŭ�����Դϴ�.
void AAuraEffectActor::ApplayEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    // Ÿ�� ������ �����Ƽ �ý��� ������Ʈ�� �����ɴϴ�.
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    // Ÿ�� ���Ϳ� �����Ƽ �ý��� ������Ʈ�� ���ٸ� �Լ��� �����մϴ�.
    if (TargetASC == nullptr)
    {
        return;
    }

    // GameplayEffectClass�� ��ȿ���� Ȯ���մϴ�.
    check(GameplayEffectClass);

    // �����÷��� ����Ʈ�� ���� ���ؽ�Ʈ�� �����մϴ�.
    FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();

    // ����Ʈ�� �ҽ��� ���� ����(this)�� �߰��մϴ�.
    EffectContextHandle.AddSourceObject(this);

    // ������ Ŭ������ Context�� ������� GameplayEffectSpec�� �����մϴ�.
    const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);

    // ������ GameplayEffectSpec�� ��� ������ Ability System Component�� �����մϴ�.
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


