// Fill out your copyright notice in the Description page of Project Settings.


#include "TankHUD.h"
#include "Components/TextBlock.h"

void UTankHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetRollbackStatus(true);
}

void UTankHUD::SetServerTime(int32 ServerTime)
{
	if (ServerTimeText)
	{
		ServerTimeText->SetText(FText::FromString(FString::FromInt(ServerTime)));
	}
}

void UTankHUD::SetRollbackStatus(bool bNewRollbackEnabled)
{
	if (bRollbackEnabled != bNewRollbackEnabled && RollbackStatusText)
	{
		FText TextToDisplay = bNewRollbackEnabled ? FText::FromString("Rollback enabled") : FText::FromString("Ping too high, rollback disabled");;
		RollbackStatusText->SetText(TextToDisplay);
		bRollbackEnabled = bNewRollbackEnabled;
	}
}

void UTankHUD::SetPingMS(int32 NewPing)
{
	if (CurrentPingMS != NewPing && PingText)
	{
		PingText->SetText(FText::FromString(FString::FromInt(NewPing)));
		CurrentPingMS = NewPing;
	}
}

UScoreBoard* UTankHUD::GetScoreBoard() const
{
	return ScoreBoard;
}

