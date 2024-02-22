// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TankPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERTANKS_API ATankPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnRep_Score() override;
	virtual void OnRep_PlayerName() override;

	void IncrementScore();

protected:
	void OnPlayerScoreUpdated();

};
