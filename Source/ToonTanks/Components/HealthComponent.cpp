// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "ToonTanks/TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawnReference = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	// not safe
	GameModeReference = Cast<ATankGameModeBase>(UGameplayStatics::GetGameMode(this));
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamager);

	Health = DefaultHealth;

	UE_LOG(LogTemp, Warning, TEXT("Health Component has started %s"), *GetOwner()->GetName())
	
}

void UHealthComponent::TakeDamager(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (FMath::IsNearlyZero(Damage) || FMath::IsNearlyZero(Health))
	{
		return;
	}

	if (!GameModeReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Game Mode is associated to HealthComponent %s"), *this->GetName())
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	//UE_LOG(LogTemp, Warning, TEXT("Health of %s : %f (after)"), *DamagedActor->GetName(), Health)

	if (FMath::IsNearlyZero(Health))
	{
		GameModeReference->ActorDied(GetOwner());
	}

	if (Cast<AActor>(PlayerPawnReference) == DamagedActor)
	{
		PlayerPawnReference->UpdateHealthBar(Health, DefaultHealth);
	}


}


