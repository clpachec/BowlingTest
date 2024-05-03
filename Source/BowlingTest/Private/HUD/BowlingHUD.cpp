// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BowlingHUD.h"
#include "Widgets/BowlingScoreWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ABowlingHUD::BeginPlay()
{
    Super::BeginPlay();

	InitializeBowlingScoreWidget();

	if (GetOwningPlayerController())
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayerController(), BowlingScoreWidget);
		GetOwningPlayerController()->bShowMouseCursor = true;
	}
}

void ABowlingHUD::InitializeBowlingScoreWidget()
{
	if (!IsValid(BowlingScoreClass))
		return;

	BowlingScoreWidget = CreateWidget<UBowlingScoreWidget>(GetOwningPlayerController(), BowlingScoreClass);
	if (!IsValid(BowlingScoreWidget))
		return;

	BowlingScoreWidget->AddToViewport();
}
