// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagingProjectile.h"
#include "MultiplayerTanks/MultiplayerTanksGameModeBase.h"
#include "MultiplayerTanks/Core/TankCharacter.h"
#include "MultiplayerTanks/Core/TankController.h"
void ADamagingProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	ATankCharacter* OwnerCharacter = Cast<ATankCharacter>(GetOwner());
	if (!OwnerCharacter)
	{ 
		return;
	}

	ATankCharacter* HitCharacter = Cast<ATankCharacter>(OtherActor);
	if (!HitCharacter)
	{
		return;
	}

	// If this shot was fired by a server, immediately apply damage
	// If this shot was fired by a client, do server rollback
	if (OwnerCharacter->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Shot overlapped on server"));

		AMultiplayerTanksGameModeBase* TanksGameMode = GetWorld()->GetAuthGameMode<AMultiplayerTanksGameModeBase>();
		if (TanksGameMode)
		{
			TanksGameMode->EliminatePlayer(HitCharacter, OwnerCharacter);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Shot overlapped on client"));

		// Temporary client authoritative code
		ATankController* TC = Cast<ATankController>(OwnerCharacter->GetController());
		if (TC)
		{
			TC->ServerTempClientAuthoritativeEliminatePlayer(HitCharacter, OwnerCharacter);
		}
	}

	Super::OnProjectileBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}