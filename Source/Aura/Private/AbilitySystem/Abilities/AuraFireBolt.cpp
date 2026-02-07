// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1){
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
							 "<Default>Launches a bolt of fire, exploding on impact and dealing: </>"
							 "<Damage>%d</>"
							 "<Default>, fire damage with a chance to burn</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), ScaledDamage, Level, ManaCost, Cooldown);
	}
	return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
							 "<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
							 "<Damage>%d</>"
							 "<Default>, fire damage with a chance to burn</>\n"
							 "<Small>Level: </><Level>%d</>\n"
							 "<Small>ManaCost: </><ManaCost>%.1f</>\n"
							 "<Small>Cooldown: </><Cooldown>%.1f</>"), FMath::Min(Level, NumProjectiles), ScaledDamage, Level, ManaCost, Cooldown);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();
	const int32 NumFireBolt = FMath::Min(GetAbilityLevel(), MaxNumProjectiles);

	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumFireBolt);

	for (FRotator& Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		// 为创建的Projectile添加GE参数
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		// 追踪
		if (IsValid(HomingTarget) && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			// 使新创建的USceneComponent可以随Projectile销毁
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = true;

		// 后续ApplyEffect在Projectile类OnSphereOverlap中
		Projectile->FinishSpawning(SpawnTransform);
	}
}
