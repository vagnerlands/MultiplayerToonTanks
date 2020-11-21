// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerControllerBase.h"
#include "GameFramework/Pawn.h"

void ATankPlayerControllerBase::SetPlayerEnabledState(bool isPlayerEnabled)
{
	if (isPlayerEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
}