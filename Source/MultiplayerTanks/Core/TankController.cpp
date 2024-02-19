// Fill out your copyright notice in the Description page of Project Settings.


#include "TankController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "MultiplayerTanks/MultiplayerTanksGameModeBase.h" // todo remove

#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDrawDebugEnabled(TEXT("mt.DrawDebug"), false, TEXT("Enable drawing debugs."));

ATankController::ATankController()
{
	bShowMouseCursor = true;
}

void ATankController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMoveToButtonPressed)
	{
		if (FMath::IsNearlyZero(TimeSinceLastMoveUpdate))
		{
			MoveToCursor();
		}

		TimeSinceLastMoveUpdate += DeltaTime;
		TimeSinceLastMoveUpdate = TimeSinceLastMoveUpdate > TimeBetweenMoveToUpdates ? 0 : TimeSinceLastMoveUpdate;
	}
}

void ATankController::ServerTempClientAuthoritativeEliminatePlayer_Implementation(ACharacter* PlayerToEliminate, ACharacter* AttackerPlayer)
{
	AMultiplayerTanksGameModeBase* TanksGameMode = GetWorld()->GetAuthGameMode<AMultiplayerTanksGameModeBase>();
	if (TanksGameMode)
	{
		TanksGameMode->EliminatePlayer(PlayerToEliminate, AttackerPlayer);
	}
}

void ATankController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("MoveTo", IE_Pressed, this, &ATankController::MoveToLocationButtonPressed);
		InputComponent->BindAction("MoveTo", IE_Released, this, &ATankController::MoveToLocationButtonReleased);
	}
}

void ATankController::MoveToLocationButtonPressed()
{
	bMoveToButtonPressed = true;
	TimeSinceLastMoveUpdate = 0.f;
}

void ATankController::MoveToLocationButtonReleased()
{
	bMoveToButtonPressed = false;
}

void ATankController::MoveToCursor()
{
	FHitResult Hit;
	bool bHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);

	if (bHit)
	{
		if (CVarDrawDebugEnabled.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), Hit.Location, 10, 8, FColor::Red, false, 4, (uint8)0U, 1.f);
		}
		
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Hit.Location);
	}
}
