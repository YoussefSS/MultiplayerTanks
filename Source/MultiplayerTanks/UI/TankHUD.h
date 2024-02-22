// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TankHUD.generated.h"

class UTextBlock;
class UScoreBoard;

UCLASS()
class MULTIPLAYERTANKS_API UTankHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetServerTime(int32 ServerTime);
	void SetRollbackStatus(bool bNewRollbackEnabled);
	void SetPingMS(int32 NewPing);

	UScoreBoard* GetScoreBoard() const;
protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "HUD")
	UTextBlock* ServerTimeText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "HUD")
	UTextBlock* RollbackStatusText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "HUD")
	UTextBlock* PingText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "HUD")
	UScoreBoard* ScoreBoard;
private:
	bool bRollbackEnabled = false;
	int32 CurrentPingMS = -1.f;

};
