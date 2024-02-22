// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreBoard.generated.h"

class UScoreBoardEntry;

UCLASS()
class MULTIPLAYERTANKS_API UScoreBoard : public UUserWidget
{
	GENERATED_BODY()

public:

	void InitializePlayerName(const FString& PlayerName, int32 InitScore);
	void SetPlayerNameLocal(const FString& PlayerName);
	void SetPlayerScore(const FString& PlayerName, int32 NewScore);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* ScoreBoardEntriesBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UScoreBoardEntry> ScoreBoardEntryClass;

	TMap<FString, int32> PlayerScores;
	TMap<FString, UScoreBoardEntry*> ScoreBoardEntries;
};
