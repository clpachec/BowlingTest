// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BowlingHUD.generated.h"

class UBowlingScoreWidget;

/**
 * 
 */
UCLASS()
class BOWLINGTEST_API ABowlingHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void InitializeBowlingScoreWidget();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UBowlingScoreWidget> BowlingScoreClass;

protected:
	UPROPERTY()
	UBowlingScoreWidget* BowlingScoreWidget;

};
