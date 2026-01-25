// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true; // 被限制在平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // 设置角色在启动时自动吸附到地面平面
	// GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 750.f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	CharacterClass = ECharacterClass::Elementalist;

	LeveUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeveUpNiagaraComponent"));
	LeveUpNiagaraComponent->SetupAttachment(RootComponent);
	LeveUpNiagaraComponent->bAutoActivate = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// 服务器在被控制器拥有时初始化Ability
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// 客户端在 PlayerState 同步时初始化Ability, 在此之前服务端角色被赋予PlayerState，意味着角色已被Possessed
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->GetPlayerLevel();

}

void AAuraCharacter::AddXP_Implementation(const int32 InXP)
{
	if (AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		PS->AddXP(InXP);
	}
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AAuraCharacter::GetXP_Implementation()
{
	if (const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		return PS->GetXP();
	}
	return 0;
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP)
{
	if (const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		return PS->LevelUpInfo->FindLevelForXP(InXP);
	}
	return 1;
}

int32 AAuraCharacter::GetAttributesPointsReward_Implementation(int32 Level) const
{
	if (const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		return PS->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
	}
	return 0;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	if (const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		return PS->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
	}
	return 0;
}

void AAuraCharacter::AddPlayerLevel_Implementation(int32 InLevel)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->AddLevel(InLevel);

	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	AuraASC->UpdateAbilityStatuses(GetPlayerLevel());

}

void AAuraCharacter::AddAttributePoints_Implementation(int32 InAttributePoints)
{
	if (AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		PS->AddAttributePoints(InAttributePoints);
	}
}

void AAuraCharacter::AddSpellPoints_Implementation(int32 InSpellPoints)
{
	if (AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		PS->AddSpellPoints(InSpellPoints);
	}
}

int32 AAuraCharacter::GetAttributesPoints_Implementation() const
{
	if (const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		return PS->GetAttributePoints();
	}
	return 0;
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	if (const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		return PS->GetSpellPoints();
	}
	return 0;
}

void AAuraCharacter::InitAbilityActorInfo()
{
	if (AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>())
	{
		// 初始化ASC
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent())->AbilityActorInfoSet();

		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AttributeSet = PS->GetAttributeSet();
		// 添加界面
		if (AAuraPlayerController* PC = GetController<AAuraPlayerController>())
		{
			if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
			{
				AuraHUD->InitOverlay(PC, PS, AbilitySystemComponent, AttributeSet);
			}
		}
		// 初始化重要属性
		InitializePrimaryAttributes();
		InitializeSecondaryAttributes();
		InitializeVitalAttributes();
	}
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector NiagaraLocation = LeveUpNiagaraComponent->GetComponentLocation();
	LeveUpNiagaraComponent->SetWorldRotation((CameraLocation - NiagaraLocation).Rotation());
	if (IsValid(LeveUpNiagaraComponent)) LeveUpNiagaraComponent->Activate(true);
}
