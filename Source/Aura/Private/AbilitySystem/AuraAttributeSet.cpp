// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(25.f);
	InitMaxMana(50.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

// 此处的clamping并没有永久地修改ASC的modifier，它仅改变了查询modifier返回的值。
// 这意味着任何修改器GameplayEffectExecutionCalculations和ModifierMagnitudeCalculations对CurrentValue的重计算都要重新clamping。
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if  (Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("before Health: %f"), NewValue);
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("after Health: %f"), NewValue);
	} else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties)
{
	// source 造成effect的原因  target effect的目标（AttributeSet的拥有者）
	FGameplayEffectContextHandle EffectContextHandle = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	EffectProperties.EffectContextHandle = EffectContextHandle;
	EffectProperties.SourceASC = SourceASC;
	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid())
	{
		AActor* SourceAvatarActor = SourceASC->GetAvatarActor();
		EffectProperties.SourceAvatarActor = SourceAvatarActor;
		AController* SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourceController = Pawn->GetController();
			}
		}
		if (SourceController != nullptr)
		{
			ACharacter* SourceCharacter = Cast<ACharacter>(SourceController->GetPawn());
			EffectProperties.SourceController = SourceController;
			EffectProperties.SourceCharacter = SourceCharacter;
		}

	}

	if (Data.Target.AbilityActorInfo.IsValid())
	{
		AActor* TargetActor = Data.Target.GetAvatarActor();
		AController* TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		EffectProperties.TargetASC = TargetASC;
		EffectProperties.TargetController = TargetController;
		EffectProperties.TargetCharacter = TargetCharacter;
		EffectProperties.TargetAvatarActor = TargetActor;
	}

}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// UE_LOG(LogTemp, Warning, TEXT("---Health: %f"), GetHealth());
		// UE_LOG(LogTemp, Warning, TEXT("---Magnitude: %f"), Data.EvaluatedData.Magnitude);


	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// 通知 Gameplay Ability System 属性值已发生变更
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}


