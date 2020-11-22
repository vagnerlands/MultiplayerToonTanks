// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"

#include "GameFramework/Actor.h"
#include "ToonTanks/Pawns/PawnTank.h"

float APawnTurret::DistanceToPlayer(APawnTank* player) const
{
	return FVector::Dist(player->GetActorLocation(), GetActorLocation());
}

APawnTank* APawnTurret::SelectClosestPlayer() const
{
	// local references for calculations
	APawnTank* AdjustedReferencePlayerOne = PlayerOnePawnReference;
	APawnTank* AdjustedReferencePlayerTwo = PlayerTwoPawnReference;

	// calculate distance from this turret to each player
	const float DistanceToPlayerOne = DistanceToPlayer(PlayerOnePawnReference);
	const float DistanceToPlayerTwo = DistanceToPlayer(PlayerTwoPawnReference);

	// if a player is out of range or is not alive, as disregard him
	if ((DistanceToPlayerOne > FireRange) || (!PlayerOnePawnReference->IsPlayerAlive()))
	{
		AdjustedReferencePlayerOne = nullptr;
	}

	if ((DistanceToPlayerTwo > FireRange) || (!PlayerTwoPawnReference->IsPlayerAlive()))
	{
		AdjustedReferencePlayerTwo = nullptr;
	}
	
	// if one player is valid, but the other not, then returns the valid player anyways
	if ((!AdjustedReferencePlayerOne) && (AdjustedReferencePlayerTwo)) return AdjustedReferencePlayerTwo;
	if ((AdjustedReferencePlayerOne) && (!AdjustedReferencePlayerTwo)) return AdjustedReferencePlayerOne;
	
	// otherwise return the "closest" player
	// note: in invalid cases, the value will be nullptr
	return (DistanceToPlayerOne < DistanceToPlayerTwo) ? AdjustedReferencePlayerOne : AdjustedReferencePlayerTwo;

}

// Called when the game starts or when spawned
void APawnTurret::BeginPlay()
{
	Super::BeginPlay();
	// create a fire rate timer handle, every 2 seconds, the method CheckFireCondition will be called
	// last true means that this will loop
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &APawnTurret::HoldOnFire, 3.f, false);

	// get the player reference
	PlayerOnePawnReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!PlayerOnePawnReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnTurret - Bad player One"))
	}

	PlayerTwoPawnReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 1));
	if (!PlayerTwoPawnReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnTurret - Bad player Two"))
	}
}

void APawnTurret::HandleDestruction()
{
	Super::HandleDestruction();

	Destroy();
}

APawnTurret::APawnTurret()
{
}

// Called every frame
void APawnTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APawnTank* player = SelectClosestPlayer())
	{
		RotateTurret(player->GetActorLocation());
	}
}

// Called to bind functionality to input
void APawnTurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APawnTurret::HoldOnFire()
{
	// starts counting fire
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &APawnTurret::CheckFireCondition, FireRate, true);
}

void APawnTurret::CheckFireCondition()
{
	if (APawnTank* player = SelectClosestPlayer())
	{
		// if player is in range and we're ready to fire, then FIRE!
		Fire();
	}
}

