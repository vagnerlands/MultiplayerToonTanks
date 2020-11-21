// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"

#include "GameFramework/Actor.h"
#include "ToonTanks/Pawns/PawnTank.h"

float APawnTurret::DistanceToPlayer() const
{
	return FVector::Dist(PlayerPawnReference->GetActorLocation(), GetActorLocation());
}

// Called when the game starts or when spawned
void APawnTurret::BeginPlay()
{
	Super::BeginPlay();
	// create a fire rate timer handle, every 2 seconds, the method CheckFireCondition will be called
	// last true means that this will loop
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &APawnTurret::HoldOnFire, 3.f, false);

	// get the player reference
	PlayerPawnReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
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

	if (!PlayerPawnReference
		|| (!PlayerPawnReference->IsPlayerAlive())
		|| (DistanceToPlayer() > FireRange))
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Turret %s rotates %s"), *GetName(), *TargetRotation.ToString())
	RotateTurret(PlayerPawnReference->GetActorLocation());
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
	// check if Player != nullptr (if null, player is dead/out of the game)
	if ((!PlayerPawnReference) 
		|| (!PlayerPawnReference->IsPlayerAlive()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is already dead!"))
		return;
	}

	if (DistanceToPlayer() <= FireRange)
	{
		// if player is in range and we're ready to fire, then FIRE!
		Fire();
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player is too far :( %s"), *PlayerPawnReference->GetName())
	}

}

