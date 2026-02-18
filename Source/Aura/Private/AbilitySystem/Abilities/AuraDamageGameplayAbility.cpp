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

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, const FVector InRadialDamageOrigin,
	bool bOverrideKnockBackDirection, const FVector KnockBackDirectionOverride,
	bool bOverrideDeathImpulseDirection, const FVector DeathImpulseDirectionOverride,
	bool bOverridePitch, float PitchOverride) const
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
	DamageEffectParams.KnockBackForceMagnitude = KnockBackForceMagnitude;
	DamageEffectParams.KnockBackChance = KnockBackChance;

	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		const FVector ToTarget = Rotation.Vector();
		// Death Impulse
		if (bOverrideDeathImpulseDirection)
		{
			FRotator DeathImpulseRotator = DeathImpulseDirectionOverride.Rotation();
			if (bOverridePitch) DeathImpulseRotator.Pitch = PitchOverride;
			DamageEffectParams.DeathImpulse = DeathImpulseRotator.Vector() * DamageEffectParams.DeathImpulseMagnitude;
		}
		else DamageEffectParams.DeathImpulse = ToTarget * DamageEffectParams.DeathImpulseMagnitude;
		// Knock Back
		if (FMath::RandRange(1, 100) < DamageEffectParams.KnockBackChance)
		{
			if (bOverrideKnockBackDirection)
			{
				FRotator KnockBackRotation = KnockBackDirectionOverride.Rotation();
				if (bOverridePitch) KnockBackRotation.Pitch = PitchOverride;
				DamageEffectParams.KnockBackForce = KnockBackRotation.Vector() * DamageEffectParams.KnockBackForceMagnitude;
			}
			else DamageEffectParams.KnockBackForce = ToTarget * DamageEffectParams.KnockBackForceMagnitude;
		}
	}

	if (bIsRadialDamage)
	{
		DamageEffectParams.bIsRadialDamage = bIsRadialDamage;
		DamageEffectParams.RadialDamageOrigin = InRadialDamageOrigin == FVector::ZeroVector ? RadialDamageOrigin : InRadialDamageOrigin;
		DamageEffectParams.RadialDamageInnerRadius = RadialDamageInnerRadius;
		DamageEffectParams.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}

	return DamageEffectParams;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}


