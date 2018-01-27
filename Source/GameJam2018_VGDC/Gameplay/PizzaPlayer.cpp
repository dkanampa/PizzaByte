// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaPlayer.h"


// Sets default values
APizzaPlayer::APizzaPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Funds = 0;
}

// Called when the game starts or when spawned
void APizzaPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APizzaPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Test buying new permit addition
	FSector sector = FSector();
	if (APizzaPlayer::PurchaseTowerInSector(&sector))
	{
		UE_LOG(LogTemp, Display, TEXT("Permit added"));
	}
	else 
	{
		UE_LOG(LogTemp, Display, TEXT("Permit not added"));
	}

}

// Called to bind functionality to input
void APizzaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Purchases a tower in the given sector. A player can only buy a tower
// if they own that sector's permit
bool APizzaPlayer::PurchaseTowerInSector(FSector * Sector)
{
	// Check if the player owns the sector's permit
	if (APizzaPlayer::hasBoughtSectorPermit(Sector->ParentDistrict)) return false;
	// Otherwise, add the permit to the player's district
	PermittedDistricts.Emplace(Sector->ParentDistrict);
	return false;
}

bool APizzaPlayer::PursueOrder(FOrder& Order) {
	// TODO
}

// Returns if this player owns a certain district's permit
bool APizzaPlayer::hasBoughtSectorPermit(FDistrict* District) {
	for (auto Permit : PermittedDistricts) {
		if (Permit == District) return true;
	}
	return false;
}

