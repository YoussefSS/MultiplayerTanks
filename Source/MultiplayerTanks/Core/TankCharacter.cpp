// Fill out your copyright notice in the Description page of Project Settings.


#include "TankCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MultiplayerTanks/Actors/Projectile.h"
#include "MultiplayerTanks/Actors/DamagingProjectile.h"
#include "MultiplayerTanks/Components/RollbackComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "TankController.h"

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
	
	// The pathfinding component is not correctly initialized on clients when a pawn is respawned
	if (!HasAuthority() && GetController())
	{
		UPathFollowingComponent* PathFollowingComp = GetController()->FindComponentByClass<UPathFollowingComponent>();
		if (PathFollowingComp)
		{
			PathFollowingComp->UpdateCachedComponents();
		}
	}
}

void ATankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATankCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATankCharacter::FireButtonPressed);
}


void ATankCharacter::FireButtonPressed()
{
	// Approach:
	// Clients within a certain ping threshold will fire a projectile locally and use server rollback to confirm the hit on the server
	// Server and Clients with a ping higher than the threshold will have their projectile fully handled by the server

	TankController = TankController ? TankController : Cast<ATankController>(GetController());
	if (TankController)
	{
		if (TankController->GetPingTooHighForRollback() || HasAuthority())
		{
			ServerFireReplicatedDamagingProjectile();
		}
		else
		{
			FireUnreplicatedDamagingProjectile();
			ServerFireVisualProjectile();
		}
	}
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

void ATankCharacter::FireUnreplicatedDamagingProjectile()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;

	World->SpawnActor<ADamagingProjectile>(UnreplicatedDamagingProjectileClass, FireLocationComponent->GetComponentLocation(), FireLocationComponent->GetComponentRotation(), ActorSpawnParams);
}

void ATankCharacter::ServerFireVisualProjectile_Implementation()
{
	MulticastFireVisualProjectile();
}

void ATankCharacter::MulticastFireVisualProjectile_Implementation()
{
	if (!IsLocallyControlled())
	{
		FireVisualProjectile();
	}
}

void ATankCharacter::ServerFireReplicatedDamagingProjectile_Implementation()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;

	World->SpawnActor<AProjectile>(ReplicatedDamagingProjectileClass, FireLocationComponent->GetComponentLocation(), FireLocationComponent->GetComponentRotation(), ActorSpawnParams);
}
