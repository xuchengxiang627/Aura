// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	// InitHealth(50.f);
	// InitMana(25.f);
	// InitMaxHealth(100.f);
	// InitMaxMana(50.f);
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

	// FAttributeSignature StrengthDelegate;
	// StrengthDelegate.BindStatic(GetStrengthAttribute);
	// TagsToAttributes.Add(Tags.Attributes_Primary_Strength, StrengthDelegate);
	//
	// FAttributeSignature IntelligenceDelegate;
	// IntelligenceDelegate.BindStatic(GetIntelligenceAttribute);
	// TagsToAttributes.Add(Tags.Attributes_Primary_Intelligence, IntelligenceDelegate);

	/** Primary Attributes*/
	TagsToAttributes.Add(Tags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(Tags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Primary_Vigor, GetVigorAttribute);

	/** Secondary Attributes*/
	TagsToAttributes.Add(Tags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);

	/** Resistance*/
	TagsToAttributes.Add(Tags.Attributes_Resistance_Fire, GetResistanceFireAttribute);
	TagsToAttributes.Add(Tags.Attributes_Resistance_Lightning, GetResistanceLightningAttribute);
	TagsToAttributes.Add(Tags.Attributes_Resistance_Arcane, GetResistanceArcaneAttribute);
	TagsToAttributes.Add(Tags.Attributes_Resistance_Physical, GetResistancePhysicalAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ResistanceFire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ResistanceLightning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ResistanceArcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ResistancePhysical, COND_None, REPNOTIFY_Always);

}

// 此处的clamping并没有永久地修改ASC的modifier，它仅改变了查询modifier返回的值。
// 这意味着任何修改器GameplayEffectExecutionCalculations和ModifierMagnitudeCalculations对CurrentValue的重计算都要重新clamping。
// 即只改变currentValue, 但每次effect应用时计算都以baseValue为基础
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if  (Attribute == GetHealthAttribute())
	{
		// UE_LOG(LogTemp, Warning, TEXT("before Health: %f"), NewValue);
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		// UE_LOG(LogTemp, Warning, TEXT("after Health: %f"), NewValue);
	} else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties)
{
	// source 造成effect的原因  target effect的目标（AttributeSet的拥有者）
	FGameplayEffectContextHandle EffectContextHandle = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	EffectProperties.EffectContextHandle = EffectContextHandle;
	EffectProperties.SourceASC = SourceASC;
	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid())
	{
		AActor* SourceAvatarActor = SourceASC->GetAvatarActor();
		EffectProperties.SourceAvatarActor = SourceAvatarActor;
		AController* SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourceController = Pawn->GetController();
			}
		}
		if (SourceController != nullptr)
		{
			ACharacter* SourceCharacter = Cast<ACharacter>(SourceController->GetPawn());
			EffectProperties.SourceController = SourceController;
			EffectProperties.SourceCharacter = SourceCharacter;
		}

	}

	if (Data.Target.AbilityActorInfo.IsValid())
	{
		AActor* TargetActor = Data.Target.GetAvatarActor();
		AController* TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		EffectProperties.TargetASC = TargetASC;
		EffectProperties.TargetController = TargetController;
		EffectProperties.TargetCharacter = TargetCharacter;
		EffectProperties.TargetAvatarActor = TargetActor;
	}

}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& EffectProperties, const float LocalIncomingDamage)
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(EffectProperties.SourceController))
	{
		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(EffectProperties.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);
		PC->ShowDamageNumber(LocalIncomingDamage, EffectProperties.TargetCharacter, bBlockedHit, bCriticalHit);
		return;
	}
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(EffectProperties.TargetController))
	{
		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(EffectProperties.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);
		PC->ShowDamageNumber(LocalIncomingDamage, EffectProperties.TargetCharacter, bBlockedHit, bCriticalHit);
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& EffectProperties)
{
	if (ICombatInterface* TargetInterface = Cast<ICombatInterface>(EffectProperties.TargetCharacter))
	{
		const ECharacterClass TargetCharacterClass = TargetInterface->Execute_GetCharacterClass(EffectProperties.TargetCharacter);

		FGameplayEventData PayLoad;
		PayLoad.EventTag = FAuraGameplayTags::Get().Attributes_Meta_IncomingXP;
		PayLoad.EventMagnitude = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(EffectProperties.TargetCharacter, TargetCharacterClass, TargetInterface->GetPlayerLevel());

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EffectProperties.SourceCharacter, FAuraGameplayTags::Get().Attributes_Meta_IncomingXP, PayLoad);
	}
}

void UAuraAttributeSet::DeBuff(const FEffectProperties& EffectProperties)
{
	// 创建GE并应用
	FGameplayEffectContextHandle EffectContextHandle = EffectProperties.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(EffectProperties.SourceAvatarActor);

	FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(EffectProperties.EffectContextHandle);
	const float DeBuffDamage = UAuraAbilitySystemLibrary::GetDeBuffDamage(EffectProperties.EffectContextHandle);
	const float DeBuffDuration = UAuraAbilitySystemLibrary::GetDeBuffDuration(EffectProperties.EffectContextHandle);
	const float DeBuffFrequency = UAuraAbilitySystemLibrary::GetDeBuffFrequency(EffectProperties.EffectContextHandle);

	const FString DeBuffName = FString::Printf(TEXT("%s DeBuff"), *DamageType.ToString());
	UGameplayEffect* DeBuffEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(*DeBuffName));
	DeBuffEffect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DeBuffEffect->Period = DeBuffFrequency;
	DeBuffEffect->DurationMagnitude = FScalableFloat(DeBuffDuration);

	// 为目标添加标签
	// 旧代码（已弃用）
	// DeBuffEffect->InheritableOwnedTagsContainer.AddTag(FAuraGameplayTags::Get().DamageTypesToDeBuffs[DamageType]);
	// 新代码（使用 UTargetTagsGameplayEffectComponent）
	UTargetTagsGameplayEffectComponent& TargetTagsComponent = DeBuffEffect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	FInheritedTagContainer TagContainer;
	const FGameplayTag DeBuffTag = FAuraGameplayTags::Get().DamageTypesToDeBuffs[DamageType];
	TagContainer.AddTag(DeBuffTag);
	if (DeBuffTag == FAuraGameplayTags::Get().DeBuff_Stun)
	{
		TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_CursorTrace);
		TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_InputHeld);
		TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_InputPressed);
	}
	TargetTagsComponent.SetAndApplyTargetTagChanges(TagContainer); // 设置目标标签

	// 堆叠策略
	DeBuffEffect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	DeBuffEffect->StackLimitCount = 1;

	// 修饰符
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.ModifierMagnitude = FScalableFloat(DeBuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();
	DeBuffEffect->Modifiers.Add(ModifierInfo);

	if (const FGameplayEffectSpec* GameplayEffectSpec = new FGameplayEffectSpec(DeBuffEffect, EffectContextHandle, 1.f))
	{
		FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(GameplayEffectSpec->GetContext().Get());
		AuraEffectContext->SetDamageType(MakeShared<FGameplayTag>(DamageType));
		EffectProperties.TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec);
	}
}

// 为什么不在之前set，因为该函数调用时，effect应用完成，但属性值还没有复制到客户端，此时再修改可以避免二次复制
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);

	if (EffectProperties.TargetCharacter->Implements<UCombatInterface>())
	{
		if (ICombatInterface::Execute_IsDead(EffectProperties.TargetCharacter))
		{
			return;
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// UE_LOG(LogTemp, Warning, TEXT("---Set: %p"), this)
		// UE_LOG(LogTemp, Warning, TEXT("---Health: %f"), GetHealth());
		// UE_LOG(LogTemp, Warning, TEXT("---Magnitude: %f"), Data.EvaluatedData.Magnitude);
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(EffectProperties);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(EffectProperties);
	}
	// UE_LOG(LogTemp, Warning, TEXT("%s %s: %f"), *EffectProperties.TargetAvatarActor->GetName(), *Data.EvaluatedData.Attribute.GetName(), Data.EvaluatedData.Attribute.GetNumericValue(this));
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	} else if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProperties)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		if (const bool bFatal = NewHealth <= 0.f; !bFatal)
		{
			if (EffectProperties.TargetAvatarActor->Implements<UCombatInterface>() &&
				!ICombatInterface::Execute_IsBeingShocked(EffectProperties.TargetAvatarActor))
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				// 激活带Effects_HitReact标签的 ability
				EffectProperties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			const FVector KnockBackForce = UAuraAbilitySystemLibrary::GetKnockBackForce(EffectProperties.EffectContextHandle);
			if (!KnockBackForce.IsNearlyZero(1.f))
			{
				EffectProperties.TargetCharacter->LaunchCharacter(KnockBackForce, true, true);
			}
		} else
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.TargetAvatarActor);
			if (CombatInterface)
			{
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(EffectProperties.EffectContextHandle));
			}
			SendXPEvent(EffectProperties);
		}
		// 显示伤害数字
		ShowFloatingText(EffectProperties, LocalIncomingDamage);
		if (UAuraAbilitySystemLibrary::IsSuccessfulDeBuff(EffectProperties.EffectContextHandle))
		{
			// 添加deBuff
			DeBuff(EffectProperties);
		}
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& EffectProperties)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);

	if (ICombatInterface *CombatInterface = Cast<ICombatInterface>(EffectProperties.SourceCharacter);
		CombatInterface && EffectProperties.SourceCharacter->Implements<UPlayerInterface>())
	{
		const int32 CurrentLevel = CombatInterface->GetPlayerLevel();
		// int32 CurrentXP = IPlayerInterface::Execute_GetXP(EffectProperties.SourceCharacter);

		IPlayerInterface::Execute_AddXP(EffectProperties.SourceCharacter, LocalIncomingXP);

		const int32 NewXP = IPlayerInterface::Execute_GetXP(EffectProperties.SourceCharacter);
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(EffectProperties.SourceCharacter, NewXP);
		const int32 NumOfLevelUp = NewLevel - CurrentLevel;
		if (NumOfLevelUp > 0)
		{
			const int32 AttributePointReward = IPlayerInterface::Execute_GetAttributesPointsReward(EffectProperties.SourceCharacter, CurrentLevel);
			const int32 SpellPointReward = IPlayerInterface::Execute_GetSpellPointsReward(EffectProperties.SourceCharacter, CurrentLevel);

			IPlayerInterface::Execute_AddPlayerLevel(EffectProperties.SourceCharacter, NumOfLevelUp);
			IPlayerInterface::Execute_AddAttributePoints(EffectProperties.SourceCharacter, AttributePointReward);
			IPlayerInterface::Execute_AddSpellPoints(EffectProperties.SourceCharacter, SpellPointReward);

			IPlayerInterface::Execute_LevelUp(EffectProperties.SourceCharacter);
			bTopOffHealth = true;
			bTopOffMana = true;
		}
	}
	// UE_LOG(LogAura, Warning, TEXT("IncomingXP: %f"), LocalIncomingXP);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// 通知 Gameplay Ability System 属性值已发生变更
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_ResistanceFire(const FGameplayAttributeData& OldResistanceFire) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ResistanceFire, OldResistanceFire);
}

void UAuraAttributeSet::OnRep_ResistanceLightning(const FGameplayAttributeData& OldResistanceLightning) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ResistanceLightning, OldResistanceLightning);
}

void UAuraAttributeSet::OnRep_ResistanceArcane(const FGameplayAttributeData& OldResistanceArcane) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ResistanceArcane, OldResistanceArcane);
}

void UAuraAttributeSet::OnRep_ResistancePhysical(const FGameplayAttributeData& OldResistancePhysical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ResistancePhysical, OldResistancePhysical);
}


