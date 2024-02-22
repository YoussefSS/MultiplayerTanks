// Fill out your copyright notice in the Description page of Project Settings.


#include "TankController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "MultiplayerTanks/UI/TankHUD.h"
#include "MultiplayerTanks/Core/TankCharacter.h"
#include "MultiplayerTanks/Components/RollbackComponent.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerTanks/UI/ScoreBoard.h"
#include "GameFramework/GameStateBase.h"

#include "MultiplayerTanks/MultiplayerTanksGameModeBase.h" // todo remove

#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDrawDebugEnabled(TEXT("mt.DrawDebug"), false, TEXT("Enable drawing debugs."));

ATankController::ATankController()
{
	bShowMouseCursor = true;
}

void ATankController::BeginPlay()
{
	Super::BeginPlay();

	InitTankHUD();
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

	CheckTimeSync(DeltaTime);
	UpdateHUDTime();
	UpdateHUDRollbackStatus();
	UpdateHUDPing();
}

void ATankController::ServerTempClientAuthoritativeEliminatePlayer_Implementation(ACharacter* PlayerToEliminate, ACharacter* AttackerPlayer)
{
	AMultiplayerTanksGameModeBase* TanksGameMode = GetWorld()->GetAuthGameMode<AMultiplayerTanksGameModeBase>();
	if (TanksGameMode)
	{
		TanksGameMode->EliminatePlayer(PlayerToEliminate, AttackerPlayer);
	}
}

float ATankController::GetServerTime() const
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

float ATankController::GetSingleTripTime() const
{
	return SingleTripTime;
}

void ATankController::InitializeScoreBoard()
{
	if (!TankHUD || !TankHUD->GetScoreBoard() || !GetWorld())
	{
		return;
	}

	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (GameState)
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (!PS)
			{
				continue;
			}

			TankHUD->GetScoreBoard()->InitializePlayerName(PS->GetPlayerName(), PS->GetScore());

			if (PlayerState && PlayerState->GetPlayerName() == PS->GetPlayerName())
			{
				TankHUD->GetScoreBoard()->SetPlayerNameLocal(PS->GetPlayerName());
			}
		}
	}
}

void ATankController::OnPlayerScoreUpdated(const FString& PlayerName, int32 NewScore)
{
	if (TankHUD && TankHUD->GetScoreBoard())
	{
		TankHUD->GetScoreBoard()->SetPlayerScore(PlayerName, NewScore);
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

void ATankController::CheckTimeSync(float DeltaTime)
{
	if (HasAuthority())
	{
		return;
	}

	TimeSinceNetworkTimeSynced += DeltaTime;
	if (TimeSinceNetworkTimeSynced >= SyncNetworkTimeFrequency)
	{
		ServerSyncNetworkTimeRequest(GetWorld()->GetTimeSeconds());
		TimeSinceNetworkTimeSynced = 0.f;
	}
}

void ATankController::ServerSyncNetworkTimeRequest_Implementation(float ClientTimeAtRequest)
{
	ClientSyncNetworkTimeResponse(ClientTimeAtRequest, GetWorld()->GetTimeSeconds());
}

void ATankController::ClientSyncNetworkTimeResponse_Implementation(float ClientTimeAtRequest, float ServerTimeAtResponse)
{
	// The time on the server is the server time reported by the server + half the time it took for that message to reach us
	SingleTripTime = (GetWorld()->GetTimeSeconds() - ClientTimeAtRequest) * 0.5f;
	float CurrentServerTime = ServerTimeAtResponse + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ATankController::InitTankHUD()
{
	if (IsLocalPlayerController() && !TankHUD && HUDWidgetAsset)
	{
		TankHUD = CreateWidget<UTankHUD>(this, HUDWidgetAsset);
		if (TankHUD)
		{
			TankHUD->AddToViewport(0);
			TankHUD->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ATankController::UpdateHUDTime()
{
	if (!IsLocalPlayerController() || !TankHUD)
	{
		return;
	}

	TankHUD->SetServerTime(FMath::FloorToInt(GetServerTime()));
}

void ATankController::UpdateHUDRollbackStatus()
{
	if (!IsLocalPlayerController() || !TankHUD || !PlayerState)
	{
		return;
	}
	
	ATankCharacter* TankCharacter = Cast<ATankCharacter>(GetPawn());
	if (!TankCharacter || !TankCharacter->RollbackComponent)
	{
		return;
	}

	bool bRollbackEnabled = (PlayerState->GetPingInMilliseconds()) < TankCharacter->RollbackComponent->GetMaxHistoryTime()*1000;
	TankHUD->SetRollbackStatus(bRollbackEnabled);
}

void ATankController::UpdateHUDPing()
{
	if (!IsLocalPlayerController() || !TankHUD || !PlayerState)
	{
		return;
	}

	TankHUD->SetPingMS(FMath::FloorToInt(PlayerState->GetPingInMilliseconds()));
}