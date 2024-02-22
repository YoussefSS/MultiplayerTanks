// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreBoardEntry.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UScoreBoardEntry::SetPlayerName(const FString& NewPlayerName)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromString(NewPlayerName));
	}
}

void UScoreBoardEntry::SetPlayerNameLocal()
{
	if (PlayerNameText)
	{
		PlayerNameText->SetColorAndOpacity(FSlateColor(FColor::Blue));
	}
		
	if (ScoreText)
	{
		ScoreText->SetColorAndOpacity(FSlateColor(FColor::Blue));
	}
}

void UScoreBoardEntry::SetScore(int32 NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(FString::FromInt(NewScore)));
	}
}
