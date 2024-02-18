// Fill out your copyright notice in the Description page of Project Settings.


#include "TankCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATankCharacter::ATankCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetUsingAbsoluteRotation(true);
	CameraArm->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraArm, CameraArm->SocketName);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	// GetCharacterMovement()->bUseAccelerationForPaths is set to true on the blueprint, needed for pathfinding on clients

	bUseControllerRotationYaw = false;
}

void ATankCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATankCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

