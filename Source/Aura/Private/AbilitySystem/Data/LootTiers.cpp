// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems() const
{
	TArray<FLootItem> ReturnLootItems;
	for (const FLootItem& Item : LootItems)
	{
		for (int32 i = 0; i < Item.MaxNumToSpawn; i++)
		{
			if (FMath::RandRange(1, 100) <= Item.ChanceToSpawn)
			{
				FLootItem NewItem;
				NewItem.LootClass = Item.LootClass;
				NewItem.bLootLevelOverride = Item.bLootLevelOverride;
				ReturnLootItems.Add(NewItem);
			}
		}
	}
	return ReturnLootItems;
}
