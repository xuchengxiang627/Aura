// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadCastAbilityInfo();

	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnSpellPointsChangedDelegate.Broadcast(AuraPlayerState->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAbilitySystemComponent *AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->AbilityStatusChanged.AddLambda([this, AuraASC](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 AbilityLevel)
	{
		if (SelectedAbility.Ability.MatchesTag(AbilityTag))
		{
			bool bSpendPointsButtonEnabled, bEquipButtonEnabled;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bSpendPointsButtonEnabled, bEquipButtonEnabled);
			FString Description, NextLevelDescription;
			AuraASC->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, Description, NextLevelDescription);
		}
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnSpellPointsChangeDelegate.AddLambda([this, AuraASC](int32 NewPoints)
	{
		OnSpellPointsChangedDelegate.Broadcast(NewPoints);

		CurrentSpellPoints = NewPoints;
		bool bSpendPointsButtonEnabled, bEquipButtonEnabled;
		SelectedAbility.Status = AuraASC->GetStatusFromAbilityTag(SelectedAbility.Ability);
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bSpendPointsButtonEnabled, bEquipButtonEnabled);
		FString Description, NextLevelDescription;
		AuraASC->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		SpellGlobeSelectedDelegate.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, Description, NextLevelDescription);
	});

	AuraASC->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	AAuraPlayerState *AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	int32 SpellPoints = AuraPlayerState->GetSpellPoints();

	UAuraAbilitySystemComponent *AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagName = AbilityTag.MatchesTag(FAuraGameplayTags::Get().Abilities_None);
	FGameplayAbilitySpec* AbilitySpec = AuraASC->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;
	if (!bTagValid || bTagName || !bSpecValid)
	{
		AbilityStatus = FAuraGameplayTags::Get().Abilities_Status_Locked;
	} else
	{
		AbilityStatus = AuraASC->GetAbilityStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	CurrentSpellPoints = SpellPoints;

	bool bSpendPointsButtonEnabled, bEquipButtonEnabled;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bSpendPointsButtonEnabled, bEquipButtonEnabled);
	FString Description, NextLevelDescription;
	AuraASC->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, Description, NextLevelDescription);

	if (bWaitingForEquippedSelection)
	{
		StopWaitingForEquippedSelectionDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
		bWaitingForEquippedSelection = false;
	}
}

void USpellMenuWidgetController::GlobeDeSelected()
{
	if (bWaitingForEquippedSelection)
	{
		StopWaitingForEquippedSelectionDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType);
		bWaitingForEquippedSelection = false;
	}

	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->ServerSpendSpellPoint(SelectedAbility.Ability);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquippedSelectionDelegate.Broadcast(AbilityType);
	bWaitingForEquippedSelection = true;

	UAuraAbilitySystemComponent *AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	const FGameplayTag SelectedStatus = AuraASC->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = AuraASC->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquippedSelection) return;
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (AbilityType != SelectedAbilityType) return;

	UAuraAbilitySystemComponent *AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& SlotTag, const FGameplayTag& PreviousSlotTag)
{
	bWaitingForEquippedSelection = false;

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlotTag;
	LastSlotInfo.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo NewSlotInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	NewSlotInfo.StatusTag = StatusTag;
	NewSlotInfo.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(NewSlotInfo);

	StopWaitingForEquippedSelectionDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeSelected();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                     bool& bSpendPointsButtonEnabled, bool& bEquipButtonEnabled)
{
	if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		bEquipButtonEnabled = true;
		bSpendPointsButtonEnabled = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Eligible))
	{
		bEquipButtonEnabled = false;
		bSpendPointsButtonEnabled = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Unlocked))
	{
		bEquipButtonEnabled = true;
		bSpendPointsButtonEnabled = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Locked))
	{
		bEquipButtonEnabled = false;
		bSpendPointsButtonEnabled = false;
	}
}
