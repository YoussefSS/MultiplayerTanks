// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerTanksGameModeBase.generated.h"

class ATankController;
/**
 * 
 */
UCLASS()
class MULTIPLAYERTANKS_API AMultiplayerTanksGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void EliminatePlayer(ACharacter* PlayerToEliminate, ACharacter* AttackerPlayer);
};
