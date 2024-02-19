// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollision"));
	ProjectileCollision->SetupAttachment(GetRootComponent());
	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	//ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileBeginOverlap);
}

void AProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

