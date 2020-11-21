// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "ToonTanks/Actors/ProjectileBase.h"
#include "ToonTanks/Components/HealthComponent.h"

#include "Engine/Public/TimerManager.h"

// Sets de fault values
APawnBase::APawnBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	// in the 
	RootComponent = CapsuleComp;

	TankMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Base Mesh"));
	TankMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Turret Mesh"));
	TurretMesh->SetupAttachment(TankMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Tank Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

}

void APawnBase::RotateTurret(FVector LookAtTarget)
{
	FVector LookAtTargetPlain = FVector(LookAtTarget.X, LookAtTarget.Y, TurretMesh->GetComponentLocation().Z);
	FVector StartLocation = TurretMesh->GetComponentLocation();

	FRotator TurretRotation = FVector(LookAtTargetPlain - StartLocation).Rotation();

	// Update Turret Mesh rotation to face towards the LookAtTarget
	TurretMesh->SetWorldRotation(TurretRotation);
}

void APawnBase::RotateTurret(FQuat Rotation)
{
	// adds rotation according to given quaternion
	TurretMesh->SetWorldRotation(Rotation);
}

void APawnBase::Fire()
{
	if ((IsLoaded) 
		&& (ProjectileClass))
	{
		AProjectileBase* tankShell = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), TurretMesh->GetComponentRotation());
		tankShell->SetOwner(this);

		IsLoaded = false;
		// create a fire rate timer handle, every 2 seconds, the method CheckFireCondition will be called
		// last true means that this will loop
		GetWorld()->GetTimerManager().SetTimer(ReloadFireTimerHandle, this, &APawnBase::ReloadFire, ReloadTime, false);
	}
}

void APawnBase::HandleDestruction()
{
	// play death effects, particles, sound and camera shade

	// call to child overrides
	// 1 - PawnTurret - Inform gamemode turret died
	// 2 - PawnTank - Inform gamemode player died (hide all components and disable player input)

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	UGameplayStatics::SpawnEmitterAtLocation(this, DeathEffect, GetActorLocation());

}
