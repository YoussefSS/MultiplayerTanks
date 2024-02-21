// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TankCharacter.generated.h"

class AProjectile;
class ADamagingProjectile;
UCLASS()
class MULTIPLAYERTANKS_API ATankCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATankCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* CameraComponent;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* FireLocationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URollbackComponent* RollbackComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AProjectile> VisualProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ADamagingProjectile> DamagingProjectileClass;



	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void FireButtonPressed();
	void FireButtonReleased();
	void FireVisualProjectile();
	void FireDamagingProjectile();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();
	
};
