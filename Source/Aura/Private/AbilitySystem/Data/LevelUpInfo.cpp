// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP)
{
	for (int32 Level = 1; Level < LevelUpInformation.Num(); Level++)
	{
		if (XP < LevelUpInformation[Level].LevelUpRequirement) // LevelUpInformation[Level].LevelUpRequirement: 升到Level+1级的最小XP
		{
			return Level;
		}
	}
	return LevelUpInformation.Num() - 1; // 最后一级不升，作为XP可达最大值
}

// [0, 300] [300, 900] [900,
// Level 1, Level 2,   Level 3
//   0        300       900