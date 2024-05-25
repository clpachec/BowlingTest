// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BowlingFrameWidget.h"
#include <Components/TextBlock.h>
#include <Components/HorizontalBox.h>
#include <Kismet/KismetTextLibrary.h>
#include <Components/EditableText.h>
#include <Kismet/KismetStringLibrary.h>

void UBowlingFrameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FrameNumberText->SetText(UKismetTextLibrary::Conv_IntToText(FrameNumber));

	ScoreEditableTextArray = { ScoreOneEditableText , ScoreTwoEditableText , ScoreThreeEditableText };

	ScoreOneEditableText->OnTextChanged.AddDynamic(this, &UBowlingFrameWidget::HandleOnTextChangedScoreOne);
	ScoreTwoEditableText->OnTextChanged.AddDynamic(this, &UBowlingFrameWidget::HandleOnTextChangedScoreTwo);
	ScoreThreeEditableText->OnTextChanged.AddDynamic(this, &UBowlingFrameWidget::HandleOnTextChangedScoreThree);

	ScoreOneEditableText->OnTextCommitted.AddDynamic(this, &UBowlingFrameWidget::HandleOnTextCommittedScoreOne);
	ScoreTwoEditableText->OnTextCommitted.AddDynamic(this, &UBowlingFrameWidget::HandleOnTextCommittedScoreTwo);
	ScoreThreeEditableText->OnTextCommitted.AddDynamic(this, &UBowlingFrameWidget::HandleOnTextCommittedScoreThree);

	if (FrameNumber == 10)
	{
		ScoreThreeHorizontalBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UBowlingFrameWidget::ResetInput()
{
	for (UEditableText* scoreEditText : ScoreEditableTextArray)
	{
		if (!IsValid(scoreEditText))
			continue;
		scoreEditText->SetText(FText::FromString(BlankChar));
			
	}
	if (IsValid(UserScoreText))
		UserScoreText->SetText(FText::FromString(BlankChar));
}

void UBowlingFrameWidget::FocusFrame()
{
	if (IsValid(ScoreOneEditableText))
		ScoreOneEditableText->SetKeyboardFocus();
}

TArray<int32> UBowlingFrameWidget::GetPoints() const
{
	TArray<int32> result;
	for (int32 i = 0; i < 3; i++)
	{
		int32 point = 0;
		if (!GetPoint(i, point))
			continue;
			
		result.Add(point);
	}
	
	return result;
}

bool UBowlingFrameWidget::GetPoint(int32 inScoreIndex, int32& outPoint) const
{
	if (!ScoreEditableTextArray.IsValidIndex(inScoreIndex))
		return false;

	FString score = ScoreEditableTextArray[inScoreIndex]->GetText().ToString();

	if (!IsValidChar(score))
		return false;

	if (IsStrike(inScoreIndex))
	{
		outPoint = 10;
	}
	else if (inScoreIndex == 1 && IsSpare())
	{
		int32 previousPoint = 0;
		if (!GetPoint(0, previousPoint))
			return false;
			
		outPoint = 10 - previousPoint;
	}
	else
	{
		outPoint = UKismetStringLibrary::Conv_StringToInt(score);
	}
		
	
	return true;
}

void UBowlingFrameWidget::UpdateTotalScore(int32 inLastFrameScore, TArray<int32> inShotPoints)
{
	if (!IsValid(ScoreOneEditableText) || !IsValidChar(ScoreOneEditableText->GetText().ToString()))
	{
		if (IsValid(UserScoreText))
			UserScoreText->SetText(FText::FromString(BlankChar));
		return;
	}
		
	CurrentScore = inLastFrameScore;

	for (int32 i = 0; i < 3; i++)
	{
		int32 point = 0;
		if (!GetPoint(i, point))
			continue;

		CurrentScore += point;
	}

	CalculateBonusPoints(inShotPoints);

	UserScoreText->SetText(UKismetTextLibrary::Conv_IntToText(CurrentScore));
}

void UBowlingFrameWidget::CalculateBonusPoints(const TArray<int32>& inShotPoints)
{
	if (FrameNumber == 10) //No bonus points in the 10 frame
		return;

	int32 bonusShots = 0;

	if (IsStrike())
	{
		bonusShots = 2;
	}
	else if (IsSpare())
	{
		bonusShots = 1;
	}

	for (int32 i = 0; i < bonusShots; i++)
	{
		if (inShotPoints.IsValidIndex(i))
			CurrentScore += inShotPoints[i];
	}
}

bool UBowlingFrameWidget::IsValidChar(const FString& inChar) const
{
	if (inChar.IsEmpty())
		return false;
	return AllowedChars.Contains(inChar);
}

void UBowlingFrameWidget::ValidateInput(const FText& inText, int32 inScoreIndex)
{
	if (!ScoreEditableTextArray.IsValidIndex(inScoreIndex))
		return;

	FString currentString = inText.ToString();
	if (currentString.IsEmpty())
	{
		ScoreEditableTextArray[inScoreIndex]->SetText(FText::FromString(BlankChar));
		return;
	}

	FString inputChar = currentString.Right(1);

	if (!IsValidChar(inputChar))
	{
		ScoreEditableTextArray[inScoreIndex]->SetText(FText::FromString(BlankChar));
		return;
	}

	
	inputChar = ValidateEdgeCases(inScoreIndex, inputChar);

	ScoreEditableTextArray[inScoreIndex]->SetText(FText::FromString(inputChar));
}

FString UBowlingFrameWidget::ValidateEdgeCases(int32 inScoreIndex, FString inputChar)
{
	if (inScoreIndex != 1 && inputChar == SpareChars) //Only the second score can be a spare
		inputChar = BlankChar;
	else if (inScoreIndex == 1 && IsStrike() && FrameNumber != 10) //you can't have a score in second if strike
		inputChar = BlankChar;
	else if (inScoreIndex == 1 && StrikeChars.Contains(inputChar) && FrameNumber != 10) //Cannot have a strike on second if not the 10 frame
		inputChar = BlankChar;
	else if (inScoreIndex == 1 && FrameNumber == 10 && StrikeChars.Contains(inputChar) && !IsStrike()) //Cannot have a strike on second in the 10 frame if the first is not a strike
		inputChar = BlankChar;

	int32 scoreOne = 0;
	if (IsValidChar(inputChar) && inScoreIndex == 1 && GetPoint(0, scoreOne))
	{
		if ((FrameNumber == 10 && !IsStrike()) || FrameNumber != 10)
		{
			int32 totalScore = scoreOne + UKismetStringLibrary::Conv_StringToInt(inputChar);
			if (totalScore == 10) //Change it to spare if it adds to 10
				inputChar = "/";
			else if (totalScore > 10) //Do not allow more than 10 score on the second score
				inputChar = BlankChar;
		}
	}	
	
	return inputChar;
}

bool UBowlingFrameWidget::IsStrike() const
{
	return IsStrike(0);
}


bool UBowlingFrameWidget::IsStrike(int32 inIndex) const
{
	if (!ScoreEditableTextArray.IsValidIndex(inIndex))
		return false;

	UEditableText* scoreEditText = ScoreEditableTextArray[inIndex];
	if (!IsValid(scoreEditText))
		return false;

	FString score = ScoreEditableTextArray[inIndex]->GetText().ToString();
	if (score.IsEmpty())
		return false;

	return StrikeChars.Contains(scoreEditText->GetText().ToString());
}

bool UBowlingFrameWidget::IsSpare() const
{
	if (!IsValid(ScoreTwoEditableText))
		return false;

	FString score = ScoreTwoEditableText->GetText().ToString();
	if (score.IsEmpty())
		return false;

	return SpareChars.Contains(ScoreTwoEditableText->GetText().ToString());
}

bool UBowlingFrameWidget::ValidCommitedText(ETextCommit::Type inCommitMethod) const
{
	return inCommitMethod == ETextCommit::OnEnter || inCommitMethod == ETextCommit::OnUserMovedFocus;
}

void UBowlingFrameWidget::HandleOnTextChangedScoreOne(const FText& inText)
{
	ValidateInput(inText, 0);
}

void UBowlingFrameWidget::HandleOnTextChangedScoreTwo(const FText& inText)
{
	ValidateInput(inText, 1);
}

void UBowlingFrameWidget::HandleOnTextChangedScoreThree(const FText& inText)
{
	ValidateInput(inText, 2);
}

void UBowlingFrameWidget::HandleOnTextCommittedScoreOne(const FText& inText, ETextCommit::Type inCommitMethod)
{
	if (!ValidCommitedText(inCommitMethod))
		return;

	if (!IsValidChar(inText.ToString()))
		return;

	if (IsStrike() && FrameNumber != 10)
	{
		if (IsValid(ScoreTwoEditableText))
			ScoreTwoEditableText->SetText(FText::FromString(BlankChar));

		if (inCommitMethod == ETextCommit::OnEnter)
			OnNextFrame.Broadcast(FrameNumber);
	}
	else
	{
		if (inCommitMethod == ETextCommit::OnEnter)
			ScoreTwoEditableText->SetKeyboardFocus();
	}

	OnUpdateScore.Broadcast(FrameNumber);
}

void UBowlingFrameWidget::HandleOnTextCommittedScoreTwo(const FText& inText, ETextCommit::Type inCommitMethod)
{
	if (!ValidCommitedText(inCommitMethod))
		return;

	if (!IsValidChar(inText.ToString()))
		return;

	if (inCommitMethod == ETextCommit::OnEnter)
	{
		if (FrameNumber == 10)
		{
			if (IsStrike() || IsSpare())
			{
				ScoreThreeEditableText->SetKeyboardFocus();
			}
		}
		else
		{
			OnNextFrame.Broadcast(FrameNumber);
		}
	}

	OnUpdateScore.Broadcast(FrameNumber);
}

void UBowlingFrameWidget::HandleOnTextCommittedScoreThree(const FText& inText, ETextCommit::Type inCommitMethod)
{
	if (!ValidCommitedText(inCommitMethod))
		return;

	OnUpdateScore.Broadcast(FrameNumber);
}
