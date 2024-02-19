// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERTANKS_API ATankController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATankController();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void ServerTempClientAuthoritativeEliminatePlayer(ACharacter* PlayerToEliminate, ACharacter* AttackerPlayer);

protected:
	void SetupInputComponent() override;

	void MoveToLocationButtonPressed();
	void MoveToLocationButtonReleased();

	void MoveToCursor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeBetweenMoveToUpdates = 0.5f;

private:
	bool bMoveToButtonPressed = false;
	float TimeSinceLastMoveUpdate = 0.f;
};
