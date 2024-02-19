// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagingProjectile.h"

void ADamagingProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If this shot was fired by a server, and we are the server, immediately apply damage
	// If this shot was fired by a client, and we are that client, do server rollback


	Super::OnProjectileBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
