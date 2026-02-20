// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1){
		return FString::Printf(TEXT("<Title>FIRE BLAST</>\n\n"
							 "<Default>Launch %d fire balls in all direction, each coming back and exploding upon return, causing </>"
							 "<Damage>%d</>"
							 "<Default> radial fire damage with a chance to burn.</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), NumFireBolts, ScaledDamage, Level, ManaCost, Cooldown);
	}
	return FString::Printf(TEXT("<Title>FIRE BLAST</>\n\n"
							 "<Default>Launch %d fire balls in all direction, each coming back and exploding upon return, causing </>"
							 "<Damage>%d</>"
							 "<Default> radial fire damage with a chance to burn.</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), NumFireBolts, ScaledDamage, Level, ManaCost, Cooldown);

}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBolts + 1);

	TArray<AAuraFireBall*> FireBalls;
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform = FTransform(Rotator, Location);
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FireBalls.Add(FireBall);
		FireBall->FinishSpawning(SpawnTransform);
	}
	return FireBalls;
}