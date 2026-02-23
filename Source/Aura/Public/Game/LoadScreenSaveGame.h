// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM()
enum ESaveSlotStatus: int8
{
	Vacant,
	EnterName,
	Taken
};
/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString SlotName = FString();
	UPROPERTY()
	int32 SlotIndex = 0;
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	UPROPERTY()
	FString MapName = FString("Default Map");
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;
};
