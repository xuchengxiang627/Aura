// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlots.Add(2, LoadSlot_2);

	LoadSlot_0->LoadSlotName = "Slot 0";
	LoadSlot_1->LoadSlotName = "Slot 1";
	LoadSlot_2->LoadSlotName = "Slot 2";
	LoadSlot_0->SlotIndex = 0;
	LoadSlot_1->SlotIndex = 1;
	LoadSlot_2->SlotIndex = 2;
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index)
{
	if (LoadSlots.Contains(Index))
	{
		return LoadSlots[Index];
	}
	return nullptr;
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, FString& EnterName)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	LoadSlots[Slot]->SetPlayerName(EnterName);
	LoadSlots[Slot]->SetMapName(AuraGameModeBase->DefaultMapName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SlotStatus = ESaveSlotStatus::Taken;
	LoadSlots[Slot]->PlayerStartTag = AuraGameModeBase->DefaultPlayerStartTag;
	AuraGameModeBase->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(2);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance());
	AuraGameInstance->LoadSlotName = LoadSlots[Slot]->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = Slot;
	AuraGameInstance->PlayerStartTag = AuraGameModeBase->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (auto& [Index, LoadSlot]: LoadSlots)
	{
		LoadSlot->EnableSelectSlotButton.Broadcast(Index != Slot);
	}
	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AAuraGameModeBase::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = ESaveSlotStatus::Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
		SelectedSlot = nullptr;
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance());
	AuraGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotName = SelectedSlot->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;
	AuraGameModeBase->TravelToMap(SelectedSlot);
	SelectedSlot = nullptr;
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (auto& [Index, LoadSlot]: LoadSlots)
	{
		if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName, Index))
		{
			ULoadScreenSaveGame* LoadScreenSaveGame = AuraGameModeBase->GetSaveSlotData(LoadSlot->LoadSlotName, Index);
			LoadSlot->SlotStatus = LoadScreenSaveGame->SaveSlotStatus;
			LoadSlot->SetPlayerName(LoadScreenSaveGame->PlayerName);
			LoadSlot->SetMapName(LoadScreenSaveGame->MapName);
			LoadSlot->SetPlayerLevel(LoadScreenSaveGame->Level);
			LoadSlot->PlayerStartTag = LoadScreenSaveGame->PlayerStartTag;
			LoadSlot->InitializeSlot();
		}
	}
}
