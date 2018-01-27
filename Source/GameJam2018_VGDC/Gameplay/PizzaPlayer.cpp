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
	// Check if the sector is already occupied by a PizzaNode or
	// if the player has insufficient funds
	if (Sector->HasPizzaNode && PurchaseTowerWithFunds(ToPurchase)) return false;
	// Else, mark that sector as occupied and create a new PizzaNode
	Sector->HasPizzaNode = true;
	APizzaNode node;
	node.Topping = ToppingToAdd;
	ToPurchase->OccupyingNode = &node;
	return true;
}

// Returns if a player has enough Funds to purchase the requested Tower
bool APizzaPlayer::PurchaseTowerWithFunds(FBlock* ToPurchase) {
	float cost = GetOwnedNodesSizeInDistrict(ToPurchase->ParentSector->ParentDistrict) * ToPurchase->ParentSector->ParentDistrict->PropertyRate;
	if (Funds - cost >= 0.0f) {
		Funds -= cost;
		return true;
	}
	return false;
}

bool APizzaPlayer::PurchaseDistrictPermit(FDistrict * District)
{
	// Check if the player owns the sector's permit
	if (APizzaPlayer::hasBoughtSectorPermit(District)) return false;
	// Otherwise, add the permit to the player's district
	PermittedDistricts.Emplace(*District);
	return false;
}

bool APizzaPlayer::IsValidPath(EPizzaTopping Topping, TArray<APizzaNode*> Path)
{
	// Loop over all nodes and verify that they satisfy the Order
	for (auto node : Path) {
		if (node->Topping == Topping) return true;
	}
	return false;

}

// Pursues an order for the player
bool APizzaPlayer::PursueOrder(FOrder& Order, TArray<APizzaNode*> Path)
{
	// Invalidate if the path does not satisfy the order
	if (!APizzaPlayer::IsValidPath(Order.PizzaType, Path)) {
		return false;
	}
	// Add payment to total Funds (TODO)
	return true;
}

// Returns if this player owns a certain district's permit
bool APizzaPlayer::hasBoughtSectorPermit(FDistrict* District) {
	for (auto Permit : PermittedDistricts) {
		if (&Permit == District) return true;
	}
	return false;
}

// Returns the number of owned nodes in this district
int32 APizzaPlayer::GetOwnedNodesSizeInDistrict(FDistrict * District)
{
	// Needs to be more concise
	int32 count = 0;
	for (auto pNode : Nodes) {
		if (pNode->ParentDistrict == District) count++;
	}
	return count;
}

