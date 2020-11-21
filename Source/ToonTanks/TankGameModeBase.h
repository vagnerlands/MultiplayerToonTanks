// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	class APawnTank* PlayerReference;
	class ATankPlayerControllerBase* PlayerControllerReference;
	int32 numberOfTurrets = 0;

	void HandleGameStart();
	void HandleGameOver(bool playerWon);
	int32 GetNumberOfTurrets();
	void RestartGame();

protected:
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	int32 StartDelay = 3;

public:
	void ActorDied(AActor* DeadActor);

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent) void GameStart();
	UFUNCTION(BlueprintImplementableEvent) void GameOver(bool playerWon);
	
};
