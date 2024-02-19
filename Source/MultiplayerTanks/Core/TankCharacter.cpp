// Fill out your copyright notice in the Description page of Project Settings.


#include "TankCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MultiplayerTanks/Actors/Projectile.h"

ATankCharacter::ATankCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetUsingAbsoluteRotation(true);
	CameraArm->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraArm, CameraArm->SocketName);

	FireLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FireLocationComponent"));
	FireLocationComponent->SetupAttachment(RootComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	// Important: GetCharacterMovement()->bUseAccelerationForPaths is set to true on the blueprint, needed for pathfinding on clients

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

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATankCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATankCharacter::FireButtonReleased);
}


void ATankCharacter::FireButtonPressed()
{
	// Todo add some timer
	LocalFire();
	ServerFire();
}

void ATankCharacter::FireButtonReleased()
{

}

void ATankCharacter::LocalFire()
{
	// Approach:
	// The projectile is NOT replicated
	// The client that fires the projectile will handle requesting the hit from the server
	// The projectile on all non-local clients and the server will just be a visual representation

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
	//SpawnParams.Instigator = this;
	
	AProjectile* FiredProjectile = World->SpawnActor<AProjectile>(ProjectileClass, FireLocationComponent->GetComponentLocation(), FireLocationComponent->GetComponentRotation());

}

void ATankCharacter::ServerFire_Implementation()
{
	MulticastFire();
}

void ATankCharacter::MulticastFire_Implementation()
{
	if (!IsLocallyControlled())
	{
		LocalFire();
	}
}