// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1){
		return FString::Printf(TEXT("<Title>ELECTROCUTE</>\n\n"
							 "<Default>Emits a beam of lightning, connecting with the target, repeatedly causing </>"
							 "<Damage>%d</>"
							 "<Default>lightning damage with a chance to stun</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), ScaledDamage, Level, ManaCost, Cooldown);
	}
	return FString::Printf(TEXT("<Title>ELECTROCUTE</>\n\n"
							 "<Default>Emits a beam of lightning, propagating to %d additional targets nearby, repeatedly causing </>"
							 "<Damage>%d</>"
							 "<Default>lightning damage with a chance to stun</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), FMath::Min(Level - 1, MaxNumShockTargets), ScaledDamage, Level, ManaCost, Cooldown);
}
