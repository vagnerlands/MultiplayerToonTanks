// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "TankPlayerControllerBase.h"
#include "TimerManager.h"
#include "Engine/World.h"

void ATankGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
}

void ATankGameModeBase::HandleGameStart()
{
	numberOfTurrets = GetNumberOfTurrets();

	UE_LOG(LogTemp, Warning, TEXT("GameMode found %d turrets in the game"), numberOfTurrets)

	PlayerReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!PlayerReference)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Pawn couldn't be found in the game!"))
	}

	PlayerControllerReference = Cast<ATankPlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerControllerReference)
	{
		PlayerControllerReference->SetPlayerEnabledState(false);

		// creates a timed event to happen 
		FTimerHandle PlayerEnableHandle;
		FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerReference, &ATankPlayerControllerBase::SetPlayerEnabledState, true);
		GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, static_cast<float>(StartDelay), false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller couldn't be found in the game!"))
	}

	GameStart();
}

void ATankGameModeBase::HandleGameOver(bool playerWon)
{
	// check if the player has destroyed all the turrets, if so, move to "Win" condition
	// if turret destroyed to the player, go to lose condition
	GameOver(playerWon);

	FTimerHandle GameRestartHandle;
	GetWorld()->GetTimerManager().SetTimer(GameRestartHandle, this, &ATankGameModeBase::RestartGame, 2.f, false);
}

void ATankGameModeBase::ActorDied(AActor* DeadActor)
{
	if (DeadActor == PlayerReference)
	{
		if (PlayerControllerReference)
		{
			PlayerControllerReference->SetPlayerEnabledState(false);
		}
		// do not use "PlayerControllerReference" after calling this method, it's no longer valid
		PlayerReference->HandleDestruction();
		PlayerControllerReference = nullptr;

		HandleGameOver(false);
	}
	else if (APawnTurret* TurretReference = Cast<APawnTurret>(DeadActor))
	{
		TurretReference->HandleDestruction();
		--numberOfTurrets;
		if (numberOfTurrets == 0)
		{
			HandleGameOver(true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Bad casting, some unpredicted object was destroyed"))
	}

}

int32 ATankGameModeBase::GetNumberOfTurrets()
{
	TArray<AActor*> Turrets;
	UGameplayStatics::GetAllActorsOfClass(this, APawnTurret::StaticClass(), Turrets);
	return Turrets.Num();
}

void ATankGameModeBase::RestartGame()
{
	// reset everything in the level
	UGameplayStatics::OpenLevel(this, FName("Main"), true);
}

