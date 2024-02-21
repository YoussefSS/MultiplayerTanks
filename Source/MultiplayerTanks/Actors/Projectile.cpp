// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;

	RootComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollision"));
	ProjectileCollision->SetupAttachment(GetRootComponent());
	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	ProjectileCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

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
	
	ProjectileStartLocation = GetActorLocation();
	//ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileBeginOverlap);
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit); // Using Hit instead of overlap as the sweep result is more consistent
}

void AProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	Destroy();
}

void AProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	Destroy();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}