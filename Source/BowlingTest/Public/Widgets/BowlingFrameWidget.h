// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BowlingFrameWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNextFrame, int32, inFrameNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateScore, int32, inFrameNumber);

class UEditableText;
class UTextBlock;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class BOWLINGTEST_API UBowlingFrameWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
    // Event dispatcher for moving to the next frame
    UPROPERTY(BlueprintAssignable)
    FOnNextFrame OnNextFrame;

    UPROPERTY(BlueprintAssignable)
	FOnUpdateScore OnUpdateScore;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableText* ScoreOneEditableText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableText* ScoreTwoEditableText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableText* ScoreThreeEditableText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* FrameNumberText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* UserScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* ScoreThreeHorizontalBox;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 FrameNumber = 0;

	int32 CurrentScore = 0;
protected:
	FString AllowedChars = "0123456789xX/";
	FString StrikeChars = "xX";
	FString SpareChars = "/";
	FString BlankChar = "_";
	UPROPERTY()
	TArray<UEditableText*> ScoreEditableTextArray;

public:
	void ResetInput();

	void FocusFrame();
	
	TArray<int32> GetPoints() const;

	bool GetPoint(int32 inScoreIndex, int32& outPoint) const;

	void UpdateTotalScore(int32 inLastFrameScore, TArray<int32> inShotPoints);

	void CalculateBonusPoints(const TArray<int32>& inShotPoints);

protected:
	bool IsValidChar(const FString& inChar) const;

	void ValidateInput(const FText& inText, int32 inScoreIndex);

	bool IsStrike() const;

	bool IsStrike(int32 inIndex) const;
	bool IsSpare() const;

	bool ValidCommitedText(ETextCommit::Type inCommitMethod) const;
	UFUNCTION()
	void HandleOnTextChangedScoreOne(const FText& inText);

	UFUNCTION()
	void HandleOnTextChangedScoreTwo(const FText& inText);
	
	UFUNCTION()
	void HandleOnTextChangedScoreThree(const FText& inText);

	UFUNCTION()
	void HandleOnTextCommittedScoreOne(const FText& inText, ETextCommit::Type inCommitMethod);

	UFUNCTION()
	void HandleOnTextCommittedScoreTwo(const FText& inText, ETextCommit::Type inCommitMethod);

	UFUNCTION()
	void HandleOnTextCommittedScoreThree(const FText& inText, ETextCommit::Type inCommitMethod);
};
