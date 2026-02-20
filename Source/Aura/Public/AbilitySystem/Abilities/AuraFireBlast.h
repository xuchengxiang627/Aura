// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAuraFireBall> FireBallClass;
public:
	virtual FString GetDescription(int32 Level) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumFireBolts = 12;

	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();
};
