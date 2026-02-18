// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddXP(const int32 InXP);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetXP();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 FindLevelForXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetAttributesPointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetSpellPointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddPlayerLevel(int32 InLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributesPoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
};
