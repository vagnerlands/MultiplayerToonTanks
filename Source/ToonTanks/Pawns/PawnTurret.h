// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToonTanks/Pawns/PawnBase.h"
#include "PawnTurret.generated.h"


class APawnTank;

/**
 * 
 */
UCLASS()
class TOONTANKS_API APawnTurret : public APawnBase
{
	GENERATED_BODY()

private:
	void HoldOnFire();
	void CheckFireCondition();

	FTimerHandle FireRateTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Fire", meta = (AllowPrivateAccess="true"))
	float FireRate = 2.f;
	UPROPERTY(EditAnywhere, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	float FireRange = 500.f;

	APawnTank* PlayerPawnReference = nullptr;

	float DistanceToPlayer() const;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Constructor
	APawnTurret();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleDestruction() override;
	
};
