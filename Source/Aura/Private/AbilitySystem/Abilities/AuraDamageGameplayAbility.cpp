// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	/**
	*SpecHandle: 태그 세트 및 크기를 설정할 게임플레이 이펙트 스펙의 핸들입니다.
	*DataTag: 설정할 태그 세트의 키(태그)입니다.
	*Magnitude: 설정할 태그 세트의 값(크기)입니다.
	*DamageSpecHandle에 해당하는 게임플레이 이펙트 스펙에 "Damage.Fire" 태그를 가진 태그 세트를 설정하고,
	*크기를 100.0으로 설정합니다.
	*/
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude1;
	Params.KnockbackChnace = KnockbackChance;
	if (IsValid(TargetActor))
	{
		 FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		 Rotation.Pitch = 45.f;
		 const FVector ToTarget = Rotation.Vector();
		 Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		 Params.KnockbackForce = ToTarget * KnockbackForceMagnitude1;
		
	}
	return Params;
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
