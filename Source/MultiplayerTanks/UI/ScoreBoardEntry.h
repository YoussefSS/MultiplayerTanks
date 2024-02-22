// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreBoardEntry.generated.h"


class UTextBlock;

UCLASS()
class MULTIPLAYERTANKS_API UScoreBoardEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetPlayerName(const FString& NewPlayerName);
	void SetPlayerNameLocal();
	void SetScore(int32 NewScore);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ScoreText;

};
