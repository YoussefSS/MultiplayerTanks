// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagingProjectile.h"
#include "MultiplayerTanks/MultiplayerTanksGameModeBase.h"
#include "MultiplayerTanks/Core/TankCharacter.h"
#include "MultiplayerTanks/Core/TankController.h"
#include "MultiplayerTanks/Components/RollbackComponent.h"
#include "Components/SphereComponent.h"

void ADamagingProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	ATankCharacter* OwnerCharacter = Cast<ATankCharacter>(GetOwner());
	ATankCharacter* HitCharacter = Cast<ATankCharacter>(OtherActor);
	bool bServerOrSpawnedLocally = HasAuthority();
	if (!OwnerCharacter || !HitCharacter || !bServerOrSpawnedLocally)
	{ 
		Super::OnProjectileBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		return;
	}

	// If this shot was fired by a server, immediately apply damage
	// If this shot was fired by a client, do server rollback
	// If lag is above the threshold, do the check on the server without rollback - the client should lead their shots in this case
	if (OwnerCharacter->HasAuthority())
	{
		AMultiplayerTanksGameModeBase* TanksGameMode = GetWorld()->GetAuthGameMode<AMultiplayerTanksGameModeBase>();
		if (TanksGameMode)
		{
			TanksGameMode->EliminatePlayer(HitCharacter, OwnerCharacter);
		}
	}
	else
	{
		ATankController* OwnerController = Cast<ATankController>(OwnerCharacter->GetController());
		if (OwnerController && OwnerCharacter->RollbackComponent)
		{
			const float ProjectileRadius = ProjectileCollision ? ProjectileCollision->GetScaledSphereRadius() : 5.f;
			const float HitTimeOnServer = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime(); 
			OwnerCharacter->RollbackComponent->ServerScoreRequest(HitCharacter, OwnerCharacter, ProjectileStartLocation, GetActorLocation(), ProjectileRadius, HitTimeOnServer);
		}
	}

	Super::OnProjectileBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}