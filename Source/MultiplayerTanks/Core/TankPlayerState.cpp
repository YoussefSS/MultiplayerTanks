// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerState.h"
#include "TankController.h"

void ATankPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	OnPlayerScoreUpdated();
}

void ATankPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	
	if (GetWorld())
	{
		ATankController* LocalController = Cast<ATankController>(GetWorld()->GetFirstPlayerController());
		if (LocalController)
		{
			LocalController->InitializeScoreBoard();
		}
	}
}

void ATankPlayerState::IncrementScore()
{
	SetScore(GetScore() + 1);

	OnPlayerScoreUpdated();
}

void ATankPlayerState::OnPlayerScoreUpdated()
{
	if (!GetWorld())
	{
		return;
	}

	ATankController* LocalController = Cast<ATankController>(GetWorld()->GetFirstPlayerController());
	if (LocalController)
	{
		LocalController->OnPlayerScoreUpdated(GetPlayerName(), GetScore());
	}
}