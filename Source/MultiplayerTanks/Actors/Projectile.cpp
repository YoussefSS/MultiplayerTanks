// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionSphere"));
	ProjectileCollision->SetupAttachment(GetRootComponent());
	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	ProjectileCollision->SetSphereRadius(18.f);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->MaxSpeed = 2000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileStartLocation = GetActorLocation();
	ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileBeginOverlap);
}

void AProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::SetProjectileGUID(const FString& NewProjectileGuid)
{
	ProjectileGuid = NewProjectileGuid;
}

FString AProjectile::GetProjectileGuid() const
{
	return ProjectileGuid;
}