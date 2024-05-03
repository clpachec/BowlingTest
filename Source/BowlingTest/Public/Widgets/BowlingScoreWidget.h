// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BowlingScoreWidget.generated.h"

class UHorizontalBox;
class UBowlingFrameWidget;
class UButton;

/**
 * 
 */
UCLASS()
class BOWLINGTEST_API UBowlingScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* ScoreHorizontalBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ResetButton;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UBowlingFrameWidget> BowlingFrameClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<UBowlingFrameWidget*> BowlingFrameWidgets;

protected:
	void InitializeFrames();

	UFUNCTION()
	void HandleNextFrame(int32 inFrameNumber);

	UFUNCTION()
	void HandleUpdateScore(int32 inFrameNumber);

	UFUNCTION()
	void HandleResetButtonClicked();
};
