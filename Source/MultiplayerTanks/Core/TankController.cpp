// Fill out your copyright notice in the Description page of Project Settings.


#include "TankController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "MultiplayerTanks/UI/TankHUD.h"
#include "MultiplayerTanks/Core/TankCharacter.h"
#include "MultiplayerTanks/Components/RollbackComponent.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerTanks/UI/ScoreBoard.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

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

	if (HasAuthority())
	{
		PollPingTooHighStatus();
	}

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
	UpdateHUDPing();
}

void ATankController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATankController, bPingTooHighForRollback);
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

bool ATankController::GetPingTooHighForRollback() const
{
	return bPingTooHighForRollback;
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

void ATankController::PollPingTooHighStatus()
{
	ATankCharacter* TankCharacter = Cast<ATankCharacter>(GetPawn());
	if (TankCharacter && TankCharacter->RollbackComponent && PlayerState)
	{
		bPingTooHighForRollback = (PlayerState->GetPingInMilliseconds()) > TankCharacter->RollbackComponent->GetMaxHistoryTime() * 1000;
	}
}

void ATankController::OnRep_PingTooHighForRollback()
{
	UpdateHUDRollbackStatus();
}

void ATankController::UpdateHUDRollbackStatus()
{
	ATankCharacter* TankCharacter = Cast<ATankCharacter>(GetPawn());
	if (TankHUD && TankCharacter && TankCharacter->RollbackComponent)
	{
		TankHUD->SetRollbackStatus(!bPingTooHighForRollback, TankCharacter->RollbackComponent->GetMaxHistoryTime() * 1000);
	}
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

			UpdateHUDRollbackStatus();
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

void ATankController::UpdateHUDPing()
{
	if (!IsLocalPlayerController() || !TankHUD || !PlayerState)
	{
		return;
	}

	TankHUD->SetPingMS(FMath::FloorToInt(PlayerState->GetPingInMilliseconds()));
}