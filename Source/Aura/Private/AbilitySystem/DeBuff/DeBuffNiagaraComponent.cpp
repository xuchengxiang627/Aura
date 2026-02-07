// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DeBuff/DeBuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDeBuffNiagaraComponent::UDeBuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDeBuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	ICombatInterface *CombatInterface = Cast<ICombatInterface>(GetOwner());
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(DeBuffTag).AddUObject(this, &UDeBuffNiagaraComponent::DeBuffTagChanged);
	}
	else if (CombatInterface) // 避免这里初始化时，有的ASC还没有来得及创建
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* ASC)
		{
			ASC->RegisterGameplayTagEvent(DeBuffTag).AddUObject(this, &UDeBuffNiagaraComponent::DeBuffTagChanged);
		});
	}

	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDeBuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDeBuffNiagaraComponent::DeBuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDeBuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
