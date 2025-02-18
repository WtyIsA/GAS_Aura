// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

void UAuraUserWidget::ResetZOrderInViewPort(int32 zOrder)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
		return;
	
	UGameViewportClient *GameViewportClient = GameInstance->GetGameViewportClient();
	if (!GameViewportClient)
		return;
}