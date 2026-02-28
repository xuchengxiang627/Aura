// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadSlot.h"

#include "Game/LoadScreenSaveGame.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	switch (SlotStatus)
	{
		case ESaveSlotStatus::Vacant:
			SetWidgetSwitcherIndex.Broadcast(0);
			break;
		case ESaveSlotStatus::EnterName:
			SetWidgetSwitcherIndex.Broadcast(1);
			break;
		case ESaveSlotStatus::Taken:
			SetWidgetSwitcherIndex.Broadcast(2);
			break;
		default:
			break;
	}
}

void UMVVM_LoadSlot::SetPlayerName(const FString& NewPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, NewPlayerName);
}

void UMVVM_LoadSlot::SetMapName(const FString& NewMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, NewMapName);
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 NewPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, NewPlayerLevel);
}
