// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "Interaction/CombatInterface.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// float Level = GetAbilityLevel();
	// if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	// {
	// 	Level = CombatInterface->GetPlayerLevel();
	// }
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams DamageEffectParams;

	DamageEffectParams.WorldContextObject = GetAvatarActorFromActorInfo();
	DamageEffectParams.DamageGameplayEffectClass = DamageEffectClass;
	DamageEffectParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	DamageEffectParams.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	DamageEffectParams.DamageType = DamageType;
	DamageEffectParams.DeBuffChance = DeBuffChance;
	DamageEffectParams.DeBuffDamage = DeBuffDamage;
	DamageEffectParams.DeBuffDuration = DeBuffDuration;
	DamageEffectParams.DeBufFrequency = DeBuffFrequency;
	DamageEffectParams.DeathImpulseMagnitude = DeathImpulseMagnitude;
	DamageEffectParams.KnockBackForceMagnitude = DeathImpulseMagnitude;
	DamageEffectParams.KnockBackChance = KnockBackChance;

	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f;
		const FVector ToTarget = Rotation.Vector();
		DamageEffectParams.DeathImpulse = ToTarget * DamageEffectParams.DeathImpulseMagnitude;
		if (FMath::RandRange(1, 100) < DamageEffectParams.KnockBackChance)
		{
			DamageEffectParams.KnockBackForce = ToTarget * DamageEffectParams.KnockBackForceMagnitude;
		}
	}

	return DamageEffectParams;
}


