// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BowlingScoreWidget.h"
#include "Widgets/BowlingFrameWidget.h"
#include <Components/HorizontalBox.h>
#include <Components/Button.h>

void UBowlingScoreWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UBowlingScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeFrames();
}

void UBowlingScoreWidget::InitializeFrames()
{
	if (!IsValid(BowlingFrameClass) || !IsValid(ScoreHorizontalBox))
		return;

	ScoreHorizontalBox->ClearChildren();
	BowlingFrameWidgets.Empty();
	for (int32 frameNumber = 1; frameNumber <= 10; frameNumber++)
	{
		UBowlingFrameWidget* frameWidget = CreateWidget<UBowlingFrameWidget>(GetOwningPlayer(), BowlingFrameClass);
		if (!IsValid(frameWidget))
			continue;

		frameWidget->FrameNumber = frameNumber;
		BowlingFrameWidgets.Add(frameWidget);

		ScoreHorizontalBox->AddChildToHorizontalBox(frameWidget);

		frameWidget->OnNextFrame.AddUniqueDynamic(this, &UBowlingScoreWidget::HandleNextFrame);

		frameWidget->OnUpdateScore.AddUniqueDynamic(this, &UBowlingScoreWidget::HandleUpdateScore);
	}

	ResetButton->OnClicked.AddUniqueDynamic(this, &UBowlingScoreWidget::HandleResetButtonClicked);
}

void UBowlingScoreWidget::HandleNextFrame(int32 inFrameNumber)
{
	if (!BowlingFrameWidgets.IsValidIndex(inFrameNumber))
		return;

	BowlingFrameWidgets[inFrameNumber]->FocusFrame();
}

void UBowlingScoreWidget::HandleUpdateScore(int32 inFrameNumber)
{
	TArray<int32> rolls;

	int32 CurrentScore = 0;
	for (int32 index = 0; index < BowlingFrameWidgets.Num(); index++)
	{
		TArray<int32> pointsAhead;
		if (BowlingFrameWidgets.IsValidIndex(index + 1))
		{
			pointsAhead.Append(BowlingFrameWidgets[index + 1]->GetPoints());
		}
		if (BowlingFrameWidgets.IsValidIndex(index + 2))
		{
			pointsAhead.Append(BowlingFrameWidgets[index + 2]->GetPoints());
		}

		BowlingFrameWidgets[index]->UpdateTotalScore(CurrentScore, pointsAhead);

		CurrentScore = BowlingFrameWidgets[index]->CurrentScore;
	}
}

void UBowlingScoreWidget::HandleResetButtonClicked()
{
	for (UBowlingFrameWidget* frameWidget : BowlingFrameWidgets)
	{
		if (!IsValid(frameWidget))
			continue;

		frameWidget->ResetInput();
	}

	if (BowlingFrameWidgets.IsValidIndex(0))
		BowlingFrameWidgets[0]->FocusFrame();
}

