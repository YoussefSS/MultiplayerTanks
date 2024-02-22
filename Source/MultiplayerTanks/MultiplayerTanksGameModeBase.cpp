// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiplayerTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "MultiplayerTanks/Core/TankController.h"
#include "MultiplayerTanks/Core/TankPlayerState.h"

void AMultiplayerTanksGameModeBase::EliminatePlayer(ACharacter* PlayerToEliminate, ACharacter* AttackerPlayer)
{
	// Eliminate the hit player
	if (PlayerToEliminate && PlayerToEliminate->GetController())
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Idx = FMath::RandRange(0, PlayerStarts.Num() - 1);

		AController* PlayerToEliminateController = PlayerToEliminate->GetController();
		PlayerToEliminate->Destroy();
		if (PlayerToEliminateController)
		{
			PlayerToEliminateController->Reset();
			RestartPlayerAtPlayerStart(PlayerToEliminateController, PlayerStarts[Idx]);
		}
	}

	// Increment the attacker player's score
	if (AttackerPlayer && AttackerPlayer->GetPlayerState())
	{
		ATankPlayerState* AttackerPlayerState = Cast<ATankPlayerState>(AttackerPlayer->GetPlayerState());
		if (AttackerPlayerState)
		{
			AttackerPlayerState->IncrementScore();
		}
	}
}