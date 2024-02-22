// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreBoard.h"
#include "ScoreBoardEntry.h"
#include "Components/VerticalBox.h"

void UScoreBoard::InitializePlayerName(const FString& PlayerName, int32 InitScore)
{
	if (PlayerScores.Contains(PlayerName))
	{
		return;
	}

	PlayerScores.Add(PlayerName, InitScore);
	if (ScoreBoardEntriesBox)
	{
		UScoreBoardEntry* NewScoreboardEntry = CreateWidget<UScoreBoardEntry>(this, ScoreBoardEntryClass);
		if (NewScoreboardEntry)
		{
			NewScoreboardEntry->SetPlayerName(PlayerName);
			NewScoreboardEntry->SetScore(InitScore);
			ScoreBoardEntriesBox->AddChildToVerticalBox(NewScoreboardEntry);
			ScoreBoardEntries.Add(PlayerName, NewScoreboardEntry);
		}
	}
}

void UScoreBoard::SetPlayerNameLocal(const FString& PlayerName)
{
	if (ScoreBoardEntries[PlayerName])
	{
		ScoreBoardEntries[PlayerName]->SetPlayerNameLocal();
	}
}

void UScoreBoard::SetPlayerScore(const FString& PlayerName, int32 NewScore)
{
	if (!PlayerScores.Contains(PlayerName))
	{
		InitializePlayerName(PlayerName, NewScore);
	}

	PlayerScores[PlayerName] = NewScore;

	if (ScoreBoardEntries[PlayerName])
	{
		ScoreBoardEntries[PlayerName]->SetScore(NewScore);
	}
}
