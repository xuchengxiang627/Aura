// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/** IEnemy Interface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** IEnemy Interface*/

	/** ICombat Interface*/
	virtual int32 GetPlayerLevel() override;
	/** ICombat Interface*/

	UPROPERTY(BlueprintReadOnly)
	bool bHighLighted = false;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;
private:
	const int32 Custom_Depth_Red = 250;

};
