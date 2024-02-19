// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "DamagingProjectile.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERTANKS_API ADamagingProjectile : public AProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
