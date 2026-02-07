#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;
/**
 * 自定义 gameplay effect context
 */
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext: public FGameplayEffectContext
{
	GENERATED_BODY()

	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsSuccessfulDeBuff() const { return bIsSuccessfulDeBuff; }
	float GetDeBuffDamage() const { return DeBuffDamage; }
	float GetDeBuffDuration() const { return DeBuffDuration; }
	float GetDeBuffFrequency() const { return DeBuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	FVector GetDeathImpulse() const { return DeathImpulse; }
	FVector GetKnockBackForce() const { return KnockBackForce; }

	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsSuccessfulDeBuff(const bool bInIsSuccessfulDeBuff) { bIsSuccessfulDeBuff = bInIsSuccessfulDeBuff; }
	void SetDeBuffDamage(const float InDeBuffDamage) { DeBuffDamage = InDeBuffDamage; }
	void SetDeBuffDuration(const float InDeBuffDuration) { DeBuffDuration = InDeBuffDuration; }
	void SetDeBuffFrequency(const float InDeBuffFrequency) { DeBuffFrequency = InDeBuffFrequency; }
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	void SetDeathImpulse(const FVector& InDeathImpulse) { DeathImpulse = InDeathImpulse; }
	void SetKnockBackForce(const FVector& InKnockBackForce) { KnockBackForce = InKnockBackForce; }

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override {
		return FAuraGameplayEffectContext::StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	UPROPERTY()
	bool bIsCriticalHit = false;
	UPROPERTY()
	bool bIsSuccessfulDeBuff = false;
	UPROPERTY()
	float DeBuffDamage = 0.f;
	UPROPERTY()
	float DeBuffDuration = 0.f;
	UPROPERTY()
	float DeBuffFrequency = 0.f;

	TSharedPtr<FGameplayTag> DamageType;
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY()
	FVector KnockBackForce = FVector::ZeroVector;
};


template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};


USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	FDamageEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
 	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();
	UPROPERTY(BlueprintReadWrite)
	float DeBuffChance = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DeBuffDamage = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DeBuffDuration = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DeBufFrequency = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	float KnockBackForceMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector KnockBackForce = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	float KnockBackChance = 0.f;
};