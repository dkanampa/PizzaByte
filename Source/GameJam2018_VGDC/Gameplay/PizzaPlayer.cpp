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
}

// Called to bind functionality to input
void APizzaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Purchases a tower in the given sector. A player can only buy a tower
// if they own that sector's permit
bool APizzaPlayer::PurchaseTowerInSector(FSector * Sector, FBlock* ToPurchase,
	EPizzaTopping ToppingToAdd)
{
	// Check if the sector is already occupied by a PizzaNode
	if (Sector->HasPizzaNode) return false;
	// Else, mark that sector as occupied and create a new PizzaNode
	Sector->HasPizzaNode = true;
	APizzaNode node;
	node.Topping = ToppingToAdd;
	ToPurchase->OccupyingNode = &node;
	return true;
}

bool APizzaPlayer::PurchaseDistrictPermit(FDistrict * District)
{
	// Check if the player owns the sector's permit
	if (APizzaPlayer::hasBoughtSectorPermit(District)) return false;
	// Otherwise, add the permit to the player's district
	PermittedDistricts.Emplace(*District);
	return false;
}

// Pursues an order for the player
bool APizzaPlayer::PursueOrder(FOrder& Order, TArray<APizzaNode*> PizzaNodes) {
	// TODO
	return false;
}

// Returns if this player owns a certain district's permit
bool APizzaPlayer::hasBoughtSectorPermit(FDistrict* District) {
	for (auto Permit : PermittedDistricts) {
		if (&Permit == District) return true;
	}
	return false;
}

