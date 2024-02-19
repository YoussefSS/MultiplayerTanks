// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiplayerTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"

void AMultiplayerTanksGameModeBase::EliminatePlayer(ACharacter* PlayerToEliminate, ACharacter* AttackerPlayer)
{
	if (!PlayerToEliminate || !PlayerToEliminate->GetController())
	{
		return;
	}

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Idx = FMath::RandRange(0, PlayerStarts.Num()-1);

	AController* PlayerToEliminateController = PlayerToEliminate->GetController();
	PlayerToEliminate->Destroy();
	if (PlayerToEliminateController)
	{
		PlayerToEliminateController->Reset();
		RestartPlayerAtPlayerStart(PlayerToEliminateController, PlayerStarts[Idx]);
	}

	// TODO increment AttackerPlayer's score
}
