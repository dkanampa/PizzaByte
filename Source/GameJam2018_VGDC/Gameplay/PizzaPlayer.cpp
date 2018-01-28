// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaPlayer.h"
#include "PizzaGameState.h"
#include "PizzaOrderManager.h"

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
	int32 cost = GetOwnedNodesSizeInDistrict(ToPurchase->ParentSector->ParentDistrict) * ToPurchase->ParentSector->ParentDistrict->PropertyRate;
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

bool APizzaPlayer::IsValidPath(FOrder Order, TArray<APizzaNode*> Path, float& ReturnedPathLength)
{
	ReturnedPathLength = 0.0f;

	if (Path.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("IsValidPath was given an empty path!"));
		return false;
	}
	
	// Calculate node-node length
	FVector PreviousNodeLocation = Path[0]->GetActorLocation();
	for (int i = 1; i < Path.Num(); i++) // Start from second node, if available
	{
		ReturnedPathLength += FVector::Distance(Path[i]->GetActorLocation(), PreviousNodeLocation);
		PreviousNodeLocation = Path[i]->GetActorLocation();
	}

	float LastStretch = FVector::Distance(Path[Path.Num() - 1]->GetActorLocation(), Order.ExactLocation);
	ReturnedPathLength += LastStretch;
	return LastStretch < MaxNodeOrderDistance;
}

// Pursues an order for the player
bool APizzaPlayer::CheckOrder(FOrder Order, TArray<APizzaNode*> Path, FString Response)
{
	// Invalidate if the path does not satisfy the order
	float PathDistance = 0.0f;
	if (!APizzaPlayer::IsValidPath(Order, Path, PathDistance)) {
		UE_LOG(LogTemp, Error, TEXT("CheckOrder has deemed the path to be invalid!"));
		return false;
	}

	// Check to make sure response matches pizza code
	if (!Response.Equals(OrderManager->GeneratePizzaCode(PathDistance, Path)))
	{
		UE_LOG(LogTemp, Log, TEXT("CheckOrder has deemed the player messed up the PizzaCode"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Order completed successfully! Giving player %s $%d"),
		*GetName(), Order.OrderCost);
	// Add payment to total funds
	Funds += Order.OrderCost;
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

void APizzaPlayer::OnBankruptcyMaxed()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s just found out they've been bankrupt too long!!"),
		*GetName());
}

void APizzaPlayer::AddOrRemoveFunds(int32 Amount)
{
	Funds += Amount;

	if (!AssertHasGameState()) return;

	// If we JUST went bankrupt
	if (Funds < 0 && !AlreadyBankrupt)
	{
		UE_LOG(LogTemp, Log, TEXT("Player %s has gone bankrupt!"), *GetName());
		GameState->UpdatePlayerBankruptcy(this);
	}
	else if (Funds > 0 && AlreadyBankrupt)
	{
		UE_LOG(LogTemp, Log, TEXT("Player %s has left bankruptcy!"), *GetName());
		GameState->UpdatePlayerBankruptcy(this, false);
	}
	
	AlreadyBankrupt = Funds < 0;
}

int32 APizzaPlayer::GetFunds()
{
	return Funds;
}

bool APizzaPlayer::AssertHasGameState()
{
	UE_CLOG(GameState == nullptr, LogTemp, Error, 
		TEXT("AssertHasGameState failed for Player %s"), *GetName());
	return GameState != nullptr;
}