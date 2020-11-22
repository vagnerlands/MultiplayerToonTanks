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

	// and create the next player - (but only once)
	UGameplayStatics::CreatePlayer(GetWorld(), 1, true);

	HandleGameStart();
}

void ATankGameModeBase::HandleGameStart()
{
	numberOfTurrets = GetNumberOfTurrets();

	UE_LOG(LogTemp, Warning, TEXT("GameMode found %d turrets in the game"), numberOfTurrets)

	GameStart();

	PlayerOneReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!PlayerOneReference)
	{
		UE_LOG(LogTemp, Error, TEXT("Player One Pawn couldn't be found in the game!"))
	}

	PlayerTwoReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 1));
	if (!PlayerTwoReference)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Two Pawn couldn't be found in the game!"))
	}

	PlayerOneControllerReference = Cast<ATankPlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerOneControllerReference)
	{
		PlayerOneControllerReference->SetPlayerEnabledState(false);

		// creates a timed event to happen 
		FTimerHandle PlayerEnableHandle;
		FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerOneControllerReference, &ATankPlayerControllerBase::SetPlayerEnabledState, true);
		GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, static_cast<float>(StartDelay), false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller couldn't be found in the game!"))
	}

	PlayerTwoControllerReference = Cast<ATankPlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 1));
	if (PlayerTwoControllerReference)
	{
		PlayerTwoControllerReference->SetPlayerEnabledState(false);

		// creates a timed event to happen 
		FTimerHandle PlayerEnableHandle;
		FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerTwoControllerReference, &ATankPlayerControllerBase::SetPlayerEnabledState, true);
		GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, static_cast<float>(StartDelay), false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller couldn't be found in the game!"))
	}
}

void ATankGameModeBase::HandleGameOver(bool playerWon)
{
	// check if the player has destroyed all the turrets, if so, move to "Win" condition
	// if turret destroyed to the player, go to lose condition
	GameOver(playerWon);

	FTimerHandle GameRestartHandle;
	GetWorld()->GetTimerManager().SetTimer(GameRestartHandle, this, &ATankGameModeBase::RestartGame, 4.f, false);
}

void ATankGameModeBase::ActorDied(AActor* DeadActor)
{
	if ((PlayerOneReference == DeadActor) || (PlayerTwoReference == DeadActor))
	{
		if (PlayerOneReference == DeadActor)
		{
			PlayerOneControllerReference->SetPlayerEnabledState(false);
			// do not use "PlayerControllerReference" after calling this method, it's no longer valid
			PlayerOneReference->HandleDestruction();
			PlayerOneControllerReference = nullptr;
		}
		if (PlayerTwoReference == DeadActor)
		{
			PlayerTwoControllerReference->SetPlayerEnabledState(false);
			// do not use "PlayerControllerReference" after calling this method, it's no longer valid
			PlayerTwoReference->HandleDestruction();
			PlayerTwoControllerReference = nullptr;
		}

		if (++DeadPlayersCount >= 2)
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

