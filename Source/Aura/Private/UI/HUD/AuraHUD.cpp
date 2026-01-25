// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if (OverlayWidgetController == nullptr)
	{
		if (OverlayWidgetControllerClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Overlay Widget Controller Class is nullptr in AuraHUD"))
			return nullptr;
		}
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		// 绑定依赖
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(
	const FWidgetControllerParams& WidgetControllerParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		if (AttributeMenuWidgetControllerClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Attribute Menu Widget Controller Class is nullptr in AuraHUD"))
			return nullptr;
		}
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		// 绑定依赖
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(
	const FWidgetControllerParams& WidgetControllerParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		if (SpellMenuWidgetControllerClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Spell Menu Widget Controller Class is nullptr in AuraHUD"))
			return nullptr;
		}
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		// 绑定依赖
		 SpellMenuWidgetController->BindCallbacksToDependencies();
	}
	return SpellMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	if (OverlayWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Overlay Widget Class is nullptr in AuraHUD"))
		return;
	}
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	// 在界面添加到屏幕之前，获得控制器之后，广播初始值
	WidgetController->BroadcastInitialValues();
	OverlayWidget->AddToViewport();
}


