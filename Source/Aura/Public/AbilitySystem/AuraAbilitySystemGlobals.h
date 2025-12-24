// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	// 这里重写是因为在创建FGameplayEffectContextHandle调用ASC的MakeEffectContext()里调用了
	// UAbilitySystemGlobals::Get().AllocGameplayEffectContext()，其中决定了新创建的GameplayEffectContext的具体类型
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
