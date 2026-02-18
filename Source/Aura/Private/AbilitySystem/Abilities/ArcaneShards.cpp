// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1){
		return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
							 "<Default>Summon shards of arcane energy, causing radial arcane damage of </>"
							 "<Damage>%d</>"
							 "<Default>at the shard origin.</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), ScaledDamage, Level, ManaCost, Cooldown);
	}
	return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
							 "<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"
							 "<Damage>%d</>"
							 "<Default>at the shard origin.</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), FMath::Min(Level, MaxNumPoints), ScaledDamage, Level, ManaCost, Cooldown);
}
