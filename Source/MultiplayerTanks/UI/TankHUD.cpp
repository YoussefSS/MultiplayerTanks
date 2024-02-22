// Fill out your copyright notice in the Description page of Project Settings.


#include "TankHUD.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UTankHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UTankHUD::SetServerTime(int32 ServerTime)
{
	if (ServerTimeText)
	{
		ServerTimeText->SetText(FText::FromString(FString::FromInt(ServerTime)));
	}
}

void UTankHUD::SetRollbackStatus(bool bNewRollbackEnabled, int32 RollbackCutoffPing)
{
	if (bRollbackEnabled != bNewRollbackEnabled && RollbackStatusText)
	{
		FText TextToDisplay = bNewRollbackEnabled ?
			FText::FromString(FString::Printf(TEXT("Ping less than %d, rollback enabled"), RollbackCutoffPing)) :
			FText::FromString(FString::Printf(TEXT("Ping greater than %d, rollback disabled"), RollbackCutoffPing));
		RollbackStatusText->SetText(TextToDisplay);

		FColor RollbackStatusTextColor = bNewRollbackEnabled ? FColor::Green : FColor::Red;
		RollbackStatusText->SetColorAndOpacity(FSlateColor(RollbackStatusTextColor));
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

