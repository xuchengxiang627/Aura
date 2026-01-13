// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

	for (auto& Pair: AuraAttributeSet->TagsToAttributes)
	{
		FGameplayTag Tag = Pair.Key;
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
		Info.AttributeValue = Pair.Value().GetNumericValue(AuraAttributeSet);
		AttributeInfoDelegate.Broadcast(Info);
	}

	// UE_LOG(LogTemp, Warning, TEXT("UAuraAttributeSet: %s"), *AuraAttributeSet->GetName())
	// UE_LOG(LogTemp, Warning, TEXT("Health: %f"), AuraAttributeSet->GetHealth())
	// UE_LOG(LogTemp, Warning, TEXT("Strength: %f"), AuraAttributeSet->GetStrength())
	// UE_LOG(LogTemp, Warning, TEXT("Armor: %f"), AuraAttributeSet->GetArmor())
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnAttributesPointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& Pair: AuraAttributeSet->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
			{
				FGameplayTag Tag = Pair.Key;
				FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
				Info.AttributeValue = Data.NewValue;
				AttributeInfoDelegate.Broadcast(Info);
			}
		);
	}
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnAttributePointsChangeDelegate.AddLambda([this](int32 NewPoints)
	{
		OnAttributesPointsChangedDelegate.Broadcast(NewPoints);
	});
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	// UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	// FGameplayAttribute Attribute = AuraAttributeSet->TagsToAttributes.FindChecked(AttributeTag)();
	// float NewValue = Attribute.GetNumericValue(AuraAttributeSet) + 1;
	// Attribute.SetNumericValueChecked(NewValue, AuraAttributeSet);
	CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)->UpgradeAttribute(AttributeTag);
}
