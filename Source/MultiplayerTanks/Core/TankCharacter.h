// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TankCharacter.generated.h"

class AProjectile;
class ADamagingProjectile;
class ATankController;

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
	TSubclassOf<ADamagingProjectile> UnreplicatedDamagingProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ADamagingProjectile> ReplicatedDamagingProjectileClass;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void FireButtonPressed();
	void FireVisualProjectile();
	void FireUnreplicatedDamagingProjectile();

	UFUNCTION(Server, Reliable)
	void ServerFireVisualProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireVisualProjectile();
	
	UFUNCTION(Server, Reliable)
	void ServerFireReplicatedDamagingProjectile();

	UPROPERTY()
	ATankController* TankController = nullptr;
};
