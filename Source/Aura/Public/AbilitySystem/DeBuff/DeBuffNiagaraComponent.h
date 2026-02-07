// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DeBuffNiagaraComponent.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class AURA_API UDeBuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UDeBuffNiagaraComponent();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DeBuffTag;

protected:
	virtual void BeginPlay() override;
	void DeBuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
