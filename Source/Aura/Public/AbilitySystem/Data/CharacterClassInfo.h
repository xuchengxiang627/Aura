// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass: uint8
{
	Elementalist,  // 元素使
	Warrior, // 战士
	Ranger, // 游侠
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes; // 这类属性根据Character的不同会不一样

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass) const;

	// 下面两类属性都是更具Primary的属性计算得来，不同Character可以共用
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
};
