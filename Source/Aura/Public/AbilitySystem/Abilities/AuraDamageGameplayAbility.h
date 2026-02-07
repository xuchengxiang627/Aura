// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FDamageEffectParams;
/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float DeBuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float DeBuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float DeBuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float DeBuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float DeathImpulseMagnitude = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float KnockBackForceMagnitude = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float KnockBackChance = 40.f;
};
