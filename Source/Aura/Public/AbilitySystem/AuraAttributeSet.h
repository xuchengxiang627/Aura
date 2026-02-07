// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	FGameplayEffectContextHandle EffectContextHandle;
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;

	FEffectProperties(){}
};

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 在这里对属性进行clamp
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	// TMap<FGameplayTag, FAttributeSignature> TagsToAttributes;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

	/** Primary Attributes */
	UPROPERTY(ReplicatedUsing=OnRep_Strength, BlueprintReadOnly, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
	UPROPERTY(ReplicatedUsing=OnRep_Intelligence, BlueprintReadOnly, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence; // 智力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);
	UPROPERTY(ReplicatedUsing=OnRep_Resilience, BlueprintReadOnly, Category = "Primary Attributes")
	FGameplayAttributeData Resilience; // 韧性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);
	UPROPERTY(ReplicatedUsing=OnRep_Vigor, BlueprintReadOnly, Category = "Primary Attributes")
	FGameplayAttributeData Vigor; // 活力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	/** Vital Attributes */
	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	UPROPERTY(ReplicatedUsing=OnRep_Mana, BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Mana; // 法力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	/** Meta Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingXP);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	/** Secondary Attributes */
	UPROPERTY(ReplicatedUsing=OnRep_Armor, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData Armor; // 与Resilience相关，护甲， 减少伤害， 提高阻挡概率
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);
	UPROPERTY(ReplicatedUsing=OnRep_ArmorPenetration, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration; // 与Resilience相关，护甲穿透，无视敌方护甲百分比，提高暴击率
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);
	UPROPERTY(ReplicatedUsing=OnRep_BlockChance, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance; // 与Armor相关, 概率将受到伤害减半
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);
	UPROPERTY(ReplicatedUsing=OnRep_CriticalHitChance, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance; // 与ArmorPenetration相关，暴击率，概率造成双倍伤害
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance);
	UPROPERTY(ReplicatedUsing=OnRep_CriticalHitDamage, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage; // 与ArmorPenetration相关，暴击伤害，暴击时额外伤害增加
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage);
	UPROPERTY(ReplicatedUsing=OnRep_CriticalHitResistance, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance; // 与Armor相关，暴击抗性，减少敌人的暴击概率
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance);
	UPROPERTY(ReplicatedUsing=OnRep_HealthRegeneration, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration; // 与Vigor相关，生命回复, 每秒回复生命值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);
	UPROPERTY(ReplicatedUsing=OnRep_ManaRegeneration, BlueprintReadOnly, Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration; // 与Intelligence相关，法力回复, 每秒回复法力值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);
	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth, BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth; // 与Vigor相关
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	UPROPERTY(ReplicatedUsing=OnRep_MaxMana, BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana; // 与Intelligence相关
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	/** Resistance */
	UPROPERTY(ReplicatedUsing=OnRep_ResistanceFire, BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData ResistanceFire; // 与Intelligence相关
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ResistanceFire);
	UPROPERTY(ReplicatedUsing=OnRep_ResistanceLightning, BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData ResistanceLightning;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ResistanceLightning);
	UPROPERTY(ReplicatedUsing=OnRep_ResistanceArcane, BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData ResistanceArcane;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ResistanceArcane);
	UPROPERTY(ReplicatedUsing=OnRep_ResistancePhysical, BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData ResistancePhysical;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ResistancePhysical);

	UFUNCTION()
	void OnRep_ResistanceFire(const FGameplayAttributeData& OldResistanceFire) const;
	UFUNCTION()
	void OnRep_ResistanceLightning(const FGameplayAttributeData& OldResistanceLightning) const;
	UFUNCTION()
	void OnRep_ResistanceArcane(const FGameplayAttributeData& OldResistanceArcane) const;
	UFUNCTION()
	void OnRep_ResistancePhysical(const FGameplayAttributeData& OldResistancePhysical) const;

private:
	void HandleIncomingDamage(const FEffectProperties& EffectProperties);
	void HandleIncomingXP(const FEffectProperties& EffectProperties);
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties);
	void ShowFloatingText(const FEffectProperties& EffectProperties, float LocalIncomingDamage);

	void SendXPEvent(const FEffectProperties& EffectProperties);
	void DeBuff(const FEffectProperties& EffectProperties);

	bool bTopOffHealth = false;
	bool bTopOffMana = false;
};
