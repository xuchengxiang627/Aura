// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/NetSerialization.h"
#include "Engine/Engine.h"
#include "Engine/NetDriver.h"


UAuraProjectileSpell::UAuraProjectileSpell()
{
	// 这里复制策略设置为复制，实例化策略设置为每个Actor实例化，Server函数才能正常在服务器上执行
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// const FVector SocketLocation1 = Cast<ICombatInterface>(GetAvatarActorFromActorInfo())->GetCombatSocketLocation();
	// UE_LOG(LogTemp, Log, TEXT("%hhd SpawnProjectile: %s"), GetAvatarActorFromActorInfo()->HasAuthority(),*SocketLocation1.ToString());
	// 这里获取客户端的Socket位置后交给服务端生成抛射物
	// 这样做是因为在专用服务器下，服务器不播放动画，无法正确获得Socket的位置
	// 这里也可以仿照UTargetDataUnderMouse利用TargetData实现
	if (IsLocallyControlled())
	{
		if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			const FVector SocketLocation = CombatInterface->Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().Montage_Attack_Weapon);
			ServerSpawnProjectile(ProjectileTargetLocation, SocketLocation);
		}
	}
}

bool UAuraProjectileSpell::ServerSpawnProjectile_Validate(const FVector& ProjectileTargetLocation,
	const FVector& SocketLocation)
{
	return true;
}

void UAuraProjectileSpell::ServerSpawnProjectile_Implementation(const FVector& ProjectileTargetLocation,
                                                                const FVector& SocketLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	FRotator SocketRotator = (ProjectileTargetLocation - SocketLocation).Rotation();
	// SocketRotator.Pitch = 0.f;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(SocketRotator.Quaternion());

	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	// 为创建的Projectile添加技能效果
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	EffectContextHandle.AddActors({Projectile});
	EffectContextHandle.AddHitResult(FHitResult());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),
	                                                                   EffectContextHandle);

	// 通过SetByCaller设置Damage的值
	// const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	// const float ScaleDamage = Damage.GetValueAtLevel(10);
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaleDamage);
	for (auto& [Tag, ScaleDamage] : DamageTypes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tag, ScaleDamage.GetValueAtLevel(10));

		Projectile->DamageEffectSpecHandle = SpecHandle;
		// 后续ApplyEffect在Projectile类OnSphereOverlap中
		Projectile->FinishSpawning(SpawnTransform);
	}
}
