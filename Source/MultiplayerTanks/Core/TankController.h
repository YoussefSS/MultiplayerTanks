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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float GetServerTime() const;
	float GetSingleTripTime() const;
	bool GetPingTooHighForRollback() const;

	void InitializeScoreBoard();

	void OnPlayerScoreUpdated(const FString& PlayerName, int32 NewScore);

	virtual void OnRep_PlayerState() override;

protected:
	virtual void BeginPlay() override;
	void SetupInputComponent() override;
	
	/* Movement */
	void MoveToLocationButtonPressed();
	void MoveToLocationButtonReleased();
	void MoveToCursor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float TimeBetweenMoveToUpdates = 0.5f;

	/* Server and client times should be in sync for server rollback */
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerSyncNetworkTimeRequest(float ClientTimeAtRequest);

	UFUNCTION(Client, Reliable)
	void ClientSyncNetworkTimeResponse(float ClientTimeAtRequest, float ServerTimeAtResponse);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NetworkTimeSync")
	float SyncNetworkTimeFrequency = 1.f;

	void PollPingTooHighStatus();

	UPROPERTY(ReplicatedUsing=OnRep_PingTooHighForRollback)
	bool bPingTooHighForRollback = false;

	UFUNCTION()
	void OnRep_PingTooHighForRollback();

	void UpdateHUDRollbackStatus();

	/* HUD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	class UTankHUD* TankHUD;

	void InitTankHUD();
	void UpdateHUDTime();
	void UpdateHUDPing();

private:
	bool bMoveToButtonPressed = false;
	float TimeSinceLastMoveUpdate = 0.f;

	float SingleTripTime = 0.f;
	float ClientServerDelta = 0.f;
	float TimeSinceNetworkTimeSynced = 0.f;

	bool bUpdateScoreBoardWhenPlayerStateReplicates = false;
};
