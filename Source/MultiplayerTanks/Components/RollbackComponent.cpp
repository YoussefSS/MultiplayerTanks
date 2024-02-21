// Fill out your copyright notice in the Description page of Project Settings.


#include "RollbackComponent.h"
#include "MultiplayerTanks/Core/TankCharacter.h"
#include "Components/CapsuleComponent.h"
#include "MultiplayerTanks/MultiplayerTanksGameModeBase.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDrawDebugFrameHistory(TEXT("rollback.DrawFrameHistory"), false, TEXT("Enable drawing frame history."));
static TAutoConsoleVariable<bool> CVarDrawDebugHitCapsule(TEXT("rollback.DrawHitCapsule"), false, TEXT("Enable drawing the hit capsule."));

URollbackComponent::URollbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void URollbackComponent::BeginPlay()
{
	Super::BeginPlay();

	TankCharacter = Cast<ATankCharacter>(GetOwner());
}


void URollbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TankCharacter || !TankCharacter->HasAuthority())
	{
		return;
	}

	SaveCurrentFramePackage();
	TrimFrameHistory();
}

void URollbackComponent::ServerScoreRequest_Implementation(ATankCharacter* HitCharacter, ATankCharacter* AttackerCharacter, const FVector_NetQuantize& ProjectileStartLocation, const FVector_NetQuantize& ProjectileHitLocation, float HitTime)
{
	AMultiplayerTanksGameModeBase* TanksGameMode = GetWorld()->GetAuthGameMode<AMultiplayerTanksGameModeBase>();
	if (!ensure(TanksGameMode) || !HitCharacter || !AttackerCharacter)
	{
		return;
	}

	bool bHitConfirmed = ConfirmHitAtTime(HitCharacter, ProjectileStartLocation, ProjectileHitLocation, HitTime);
	if (bHitConfirmed)
	{
		TanksGameMode->EliminatePlayer(HitCharacter, AttackerCharacter);
	}
}

void URollbackComponent::DrawCapsuleAtLocation(const FVector& CapsuleLocation, FColor Color, float TimeToDraw)
{
	if (!TankCharacter || !TankCharacter->GetCapsuleComponent())
	{
		return;
	}

	DrawDebugCapsule(GetWorld(),
		CapsuleLocation,
		TankCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		TankCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		FQuat::Identity,
		Color,
		false,
		TimeToDraw,
		0.f,
		1.f
	);
}

bool URollbackComponent::ConfirmHitAtTime(ATankCharacter* HitCharacter, const FVector_NetQuantize& ProjectileStartLocation, const FVector_NetQuantize& ProjectileHitLocation, float HitTime)
{
	FVector CapsuleLocationToTest;
	bool bValidCapsuleLocationFound = GetCapsuleLocationAtTime(HitCharacter, HitTime, CapsuleLocationToTest);
	if (!bValidCapsuleLocationFound)
	{
		return false;
	}

	bool bTestSuccessful = TraceAgainstCapsule(HitCharacter, ProjectileStartLocation, ProjectileHitLocation, CapsuleLocationToTest);

	if (CVarDrawDebugHitCapsule.GetValueOnGameThread())
	{
		FColor CapsuleColor = bTestSuccessful ? FColor::Green : FColor::Red;
		DrawCapsuleAtLocation(CapsuleLocationToTest, CapsuleColor, 15.f);
		DrawDebugPoint(GetWorld(), ProjectileHitLocation, 3.f, FColor::Blue, false, 15.f);
	}

	// Do line test
	return bTestSuccessful;
}

bool URollbackComponent::GetCapsuleLocationAtTime(ATankCharacter* HitCharacter, float Time, FVector& OutCapsuleLocation)
{
	if (!HitCharacter || !HitCharacter->RollbackComponent || HitCharacter->RollbackComponent->FrameHistory.IsEmpty())
	{
		return false;
	}

	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->RollbackComponent->FrameHistory;
	const float OldestNodeTime = History.GetTail()->GetValue().Time;
	if (Time < OldestNodeTime)
	{
		return false;
	}

	// CurrentNode starts at the newest node and will go back in history
	auto CurrentNode = History.GetHead();
	if (Time >= CurrentNode->GetValue().Time)
	{
		OutCapsuleLocation = CurrentNode->GetValue().TankLocation;
		return true;
	}

	while (CurrentNode->GetNextNode() != nullptr)
	{
		// March back until: OlderNodeTime <= HitTime <= NewerNodeTime
		const float NewerNodeTime = CurrentNode->GetValue().Time;
		const float OlderNodeTime = CurrentNode->GetNextNode()->GetValue().Time;
		if (OlderNodeTime <= Time && Time <= NewerNodeTime)
		{
			// Interpolate between the two nodes
			const float Distance = NewerNodeTime - OlderNodeTime;
			const float InterpRatio = FMath::Clamp((Time - OlderNodeTime) / Distance, 0.f, 1.f);
			OutCapsuleLocation = FMath::VInterpTo(CurrentNode->GetNextNode()->GetValue().TankLocation, CurrentNode->GetValue().TankLocation, 1.f, InterpRatio);
			return true;
		}

		CurrentNode = CurrentNode->GetNextNode();
	}

	return false;
}

bool URollbackComponent::TraceAgainstCapsule(ATankCharacter* HitCharacter, const FVector_NetQuantize& ProjectileStartLocation, const FVector_NetQuantize& ProjectileHitLocation, const FVector& CapsuleCenter)
{
	if (!HitCharacter || !HitCharacter->GetCapsuleComponent())
	{
		return false;
	}

	// Step 1: Check that there are no walls in the way
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, ProjectileStartLocation, ProjectileHitLocation, ECollisionChannel::ECC_Visibility); // Pawns ignore ECC_Visibility
	if (Hit.bBlockingHit)
	{
		return false;
	}

	// Step 2: Check that the hit location is within the capsule
	const float CapsuleHalfHeight = HitCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = HitCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector CapsuleStart = CapsuleCenter + FVector(0, 0, -CapsuleHalfHeight);
	FVector CapsuleEnd = CapsuleCenter + FVector(0, 0, CapsuleHalfHeight);

	float Distance = FMath::PointDistToSegment(ProjectileHitLocation, CapsuleStart, CapsuleEnd);

	return Distance <= CapsuleRadius + TraceAgainstCapsuleRadiusTolerance;
}

float URollbackComponent::GetMaxHistoryTime() const
{
	return MaxHistoryTime;
}

void URollbackComponent::SaveCurrentFramePackage()
{
	FFramePackage CurrentFramePackage;
	CurrentFramePackage.Time = GetWorld()->GetTimeSeconds();
	CurrentFramePackage.TankLocation = TankCharacter->GetActorLocation();

	if (CVarDrawDebugFrameHistory.GetValueOnGameThread())
	{
		DrawCapsuleAtLocation(CurrentFramePackage.TankLocation, FColor::Cyan, MaxHistoryTime);
	}
	
	FrameHistory.AddHead(CurrentFramePackage);
}

void URollbackComponent::TrimFrameHistory()
{
	float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
	while (HistoryLength > MaxHistoryTime)
	{
		FrameHistory.RemoveNode(FrameHistory.GetTail());
		HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
	}
}
