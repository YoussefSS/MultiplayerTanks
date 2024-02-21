// Fill out your copyright notice in the Description page of Project Settings.


#include "TankCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MultiplayerTanks/Actors/Projectile.h"
#include "MultiplayerTanks/Actors/DamagingProjectile.h"
#include "MultiplayerTanks/Components/RollbackComponent.h"

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

	RollbackComponent = CreateDefaultSubobject<URollbackComponent>(TEXT("RollbackComponent"));

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
	// Approach:
	// Projectiles are NOT replicated - I went with this approach to get an immediate firing response on clients even with high lag
	// Server-rollback will be used to compensate for high lag
	// 
	// The client that fires the projectile will handle requesting the hit from the server
	// The projectile on all non-local clients and the server will just be a visual representation

	
	FireDamagingProjectile();
	ServerFire();
}

void ATankCharacter::FireButtonReleased()
{
	// Todo add some timer so tanks can't rapidly fire
}

void ATankCharacter::FireVisualProjectile()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;

	World->SpawnActor<AProjectile>(VisualProjectileClass, FireLocationComponent->GetComponentLocation(), FireLocationComponent->GetComponentRotation(), ActorSpawnParams);
}

void ATankCharacter::FireDamagingProjectile()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;

	World->SpawnActor<ADamagingProjectile>(DamagingProjectileClass, FireLocationComponent->GetComponentLocation(), FireLocationComponent->GetComponentRotation(), ActorSpawnParams);
}

void ATankCharacter::ServerFire_Implementation()
{
	MulticastFire();
}

void ATankCharacter::MulticastFire_Implementation()
{
	if (!IsLocallyControlled())
	{
		FireVisualProjectile();
	}
}