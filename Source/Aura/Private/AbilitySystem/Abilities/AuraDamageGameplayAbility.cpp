// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		/**
		*SpecHandle: �±� ��Ʈ �� ũ�⸦ ������ �����÷��� ����Ʈ ������ �ڵ��Դϴ�.
		*DataTag: ������ �±� ��Ʈ�� Ű(�±�)�Դϴ�.
		*Magnitude: ������ �±� ��Ʈ�� ��(ũ��)�Դϴ�.
		*DamageSpecHandle�� �ش��ϴ� �����÷��� ����Ʈ ���忡 "Damage.Fire" �±׸� ���� �±� ��Ʈ�� �����ϰ�,
		*ũ�⸦ 100.0���� �����մϴ�.
		*/
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType)
{
	checkf(DamageTypes.Contains(DamageType), TEXT("GameplayAbilit [%s] does not contain DamageType [%s]"), *GetNameSafe(this), *DamageType.ToString());
	return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}