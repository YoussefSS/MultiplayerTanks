// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagingProjectile.h"
#include "MultiplayerTanks/MultiplayerTanksGameModeBase.h"
#include "MultiplayerTanks/Core/TankCharacter.h"
#include "MultiplayerTanks/Core/TankController.h"
#include "MultiplayerTanks/Components/RollbackComponent.h"

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
	// If lag is above the threshold, do the check on the server without rollback - the client should lead their shots in this case
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

		ATankController* OwnerController = Cast<ATankController>(OwnerCharacter->GetController());
		if (OwnerController && OwnerCharacter->RollbackComponent)
		{
			const float HitTimeOnServer = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime(); 
			OwnerCharacter->RollbackComponent->ServerScoreRequest(HitCharacter, OwnerCharacter, ProjectileStartLocation, SweepResult.ImpactPoint, HitTimeOnServer);
		}
	}

	Super::OnProjectileBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ADamagingProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
	// If lag is above the threshold, do the check on the server without rollback - the client should lead their shots in this case
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

		ATankController* OwnerController = Cast<ATankController>(OwnerCharacter->GetController());
		if (OwnerController && OwnerCharacter->RollbackComponent)
		{
			const float HitTimeOnServer = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime();
			OwnerCharacter->RollbackComponent->ServerScoreRequest(HitCharacter, OwnerCharacter, ProjectileStartLocation, Hit.ImpactPoint, HitTimeOnServer);
		}
	}

	Super::OnProjectileHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
