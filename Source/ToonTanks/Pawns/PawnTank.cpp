// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTank.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

void APawnTank::CalculateMoveInput(float Value)
{
	MoveDirection = FVector(
		Value*MoveSpeed*GetWorld()->DeltaTimeSeconds,  // how much we're moving
		0,0 // avoid the tank to strafe, no sideways movement
	);
}

void APawnTank::CalculateRotateInput(float Value)
{
	float RotateAmount = Value * RotateSpeed * GetWorld()->DeltaTimeSeconds;
	FRotator rotation = FRotator(0, RotateAmount, 0);
	RotationDirection = FQuat(rotation);
}

void APawnTank::CalculateTurretRotationOnX(float Value)
{
	angleOnX = Value;
}

void APawnTank::CalculateTurretRotationOnY(float Value)
{
	angleOnY = Value * -1;
}

void APawnTank::Move()
{
	AddActorLocalOffset(MoveDirection, true);
}

void APawnTank::Rotate()
{
	AddActorLocalRotation(RotationDirection, true);
}

// Called when the game starts or when spawned
void APawnTank::BeginPlay()
{
	Super::BeginPlay();

	SelfReference = Cast<APlayerController>(GetController());
}

void APawnTank::HandleDestruction()
{
	Super::HandleDestruction();

	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);

	isPlayerAlive = false;
}

APawnTank::APawnTank()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Tank Spring Arm"));;
	SpringArm->SetupAttachment(RootComponent);
	CameraView = CreateDefaultSubobject<UCameraComponent>(TEXT("Tank Camera View"));;
	CameraView->SetupAttachment(SpringArm);
}

// Called every frame
void APawnTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Rotate();
	Move();

	// current vehicle heading
	const float actorFaceAngle = GetActorRotation().Yaw;

	if ((!FMath::IsNearlyEqual(0.0, angleOnX, 0.01)) 
		|| (!FMath::IsNearlyEqual(0.0, angleOnY, 0.01)))
	{
		// creates a vector for the joystick
		FVector joystickAngleVector(angleOnX, angleOnY, 0.);
		// find the cosine between the joystick angle against "forward" vector
		float angleCosine = joystickAngleVector.DotProduct(joystickAngleVector, FVector(0.,1.,0.));
		// find the angle in degrees 
		float angle = acos(angleCosine) * 180.f / 3.14159265f;
		// little patch to trick the angle, for this calculation, if X is negative, we must invert the angle (other quadrant)
		if (angleOnX < 0.0) 
			angle *= -1.0;
		// Updates the turret angle
		RotateTurret(FQuat(FRotator(0.f, actorFaceAngle + angle, 0.f)));
		// keep firing 
		APawnTank::Fire();
	}
	else
	{
		// avoid the turret to be pointing to the last direction, just reset it and make it aims
		// to the current vehicle direction
		RotateTurret(FQuat(FRotator(0.f, actorFaceAngle, 0.f)));
	}
}

// Called to bind functionality to input
void APawnTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APawnTank::CalculateMoveInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawnTank::CalculateRotateInput);
	PlayerInputComponent->BindAxis("RotateTurretOnX", this, &APawnTank::CalculateTurretRotationOnX);
	PlayerInputComponent->BindAxis("RotateTurretOnY", this, &APawnTank::CalculateTurretRotationOnY);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APawnTank::Fire);

}

