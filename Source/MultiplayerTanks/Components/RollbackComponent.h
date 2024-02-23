// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RollbackComponent.generated.h"

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time = 0;

	UPROPERTY()
	FVector TankLocation = FVector::ZeroVector;
};

class ATankCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERTANKS_API URollbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URollbackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(ATankCharacter* HitCharacter, ATankCharacter* AttackerCharacter, const FVector_NetQuantize& ProjectileStartLocation, const FVector_NetQuantize& ProjectileLocationAtImpact, float ProjectileRadius, float HitTime);

	float GetMaxHistoryTime() const;

protected:
	virtual void BeginPlay() override;

	void SaveCurrentFramePackage();
	void TrimFrameHistory();
	void DrawCapsuleAtLocation(const FVector& CapsuleLocation, FColor Color, float TimeToDraw);

	bool ConfirmHitAtTime(ATankCharacter* HitCharacter, const FVector_NetQuantize& ProjectileStartLocation, const FVector_NetQuantize& ProjectileLocationAtImpact, float ProjectileRadius, float HitTime);

	/* returns true if a valid capsule location was found at "Time"
	* "OutCapsuleLocation" will be set to the capsule location at "Time" */
	bool GetCapsuleLocationAtTime(ATankCharacter* HitCharacter, float Time, FVector& OutCapsuleLocation);

	/* returns true if the test was successful, i.e. the hit is confirmed */
	bool TraceAgainstCapsule(ATankCharacter* HitCharacter, const FVector_NetQuantize& ProjectileStartLocation, const FVector_NetQuantize& ProjectileLocationAtImpact, float ProjectileRadius, const FVector& CapsuleCenter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHistoryTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceAgainstCapsuleRadiusTolerance = 1.f;


private:
	UPROPERTY()
	ATankCharacter* TankCharacter;

	/* Head = newest node, Tail = oldest node */
	TDoubleLinkedList<FFramePackage> FrameHistory;
		
};
