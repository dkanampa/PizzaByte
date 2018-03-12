// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaPlayer.h"
#include "PizzaGameState.h"
#include "PizzaOrderManager.h"
#include "PizzaController.h"
#include "../GenericUsefulFunctions.h"

// Sets default values
APizzaPlayer::APizzaPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Funds = 0;

	/**
	 * Populate default presets
	 */
	TArray<TSubclassOf<AActor>> PresetDisabled; // Empty = nothing selectable
	SelectablePresets.Add(FActionModePreset(EActionMode::Disabled, PresetDisabled));

	TArray<TSubclassOf<AActor>> PresetOrderSelection;
	PresetOrderSelection.Add(AOrderPopup::StaticClass());
	SelectablePresets.Add(FActionModePreset(EActionMode::OrderSelection, PresetOrderSelection));

	TArray<TSubclassOf<AActor>> PresetOrderChaining; 
	PresetOrderChaining.Add(APizzaNode::StaticClass());
	SelectablePresets.Add(FActionModePreset(EActionMode::OrderChaining, PresetOrderChaining));

	TArray<TSubclassOf<AActor>> PresetPlacing; 
	PresetPlacing.Add(APizzaNode::StaticClass());
	SelectablePresets.Add(FActionModePreset(EActionMode::Placing, PresetPlacing));

	TArray<TSubclassOf<AActor>> PresetSelling; 
	PresetSelling.Add(APizzaNode::StaticClass());
	SelectablePresets.Add(FActionModePreset(EActionMode::Selling, PresetSelling));
}

// Called when the game starts or when spawned
void APizzaPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Makes sure ValidSelectables is right, too
	UpdateActionMode(EActionMode::OrderSelection);

	PizzaController = Cast<APizzaController>(GetController());
	if (PizzaController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PizzaPlayer controller is not a subclass of PizzaController! Is '%s'"),
			*GetController()->GetClass()->GetName());
	}
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
	
	InputComponent->BindAction("Select", IE_Pressed, this, &APizzaPlayer::StartSelect);
	InputComponent->BindAction("Select", IE_Released, this, &APizzaPlayer::StopSelect);

}

void APizzaPlayer::StartSelect()
{
	if (PizzaController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Select button clicked but no pizza controller available!"));
		return;
	}

	FHitResult HitResult;
	if (PizzaController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, 
		true, HitResult))
	{
		AActor* HitSelectable = nullptr;

		// Make sure thing is selectable:
		for (const TSubclassOf<AActor>& Selectable : ValidSelectables)
		{
			if (HitResult.GetActor()->GetClass()->IsChildOf(Selectable.Get()))
				HitSelectable = HitResult.GetActor();
		}

		if (HitSelectable == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Clicked on un-selectable: '%s' (Mode: %s)"),
				*HitResult.GetActor()->GetName(), *UsefulFunctions::EnumToString(FString("EActionMode"), ActionMode));
		}
		else
		{
			// Selectable clicked: perform necessary behavior
			// This feels a bit ugly but I can't think of any better way.
			// Feel free to refactor.

			if (AOrderPopup* OrderPopup = Cast<AOrderPopup>(HitSelectable))
			{
				// Clear selection
				SelectedPopup->IsSelected = false;
				SelectedPopup = OrderPopup;
				OrderPopup->IsSelected = true;
			}
			else if (APizzaNode* PizzaNode = Cast<APizzaNode>(HitSelectable))
			{
				PizzaNode->IsSelected = true;
				SelectedNodes.AddUnique(PizzaNode);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Selection behavior is undefined for '%s'"),
					*HitSelectable->GetClass()->GetName());
			}
		}
	}
}

void APizzaPlayer::StopSelect()
{

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

	UE_LOG(LogTemp, Log, TEXT("Path calculated that distance between %s and %s is: %f"),
		*Path[Path.Num() - 1]->GetActorLocation().ToString(),
		*Order.ExactLocation.ToString(),
		LastStretch);

	UE_LOG(LogTemp, Log, TEXT("Comparing %.3f < %.3f..."), LastStretch, MaxNodeOrderDistance);

	return LastStretch < MaxNodeOrderDistance;
}

// Pursues an order for the player
bool APizzaPlayer::CheckOrder(FString Response)
{
	// Invalidate if the path does not satisfy the order
	float PathDistance = 0.0f;
	if (!APizzaPlayer::IsValidPath(CurrentOrder, SelectedNodes, PathDistance)) {
		UE_LOG(LogTemp, Warning, TEXT("CheckOrder has deemed the path to be invalid!"));
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Path looks good! Checking code..."));
	}

	// Check to make sure response matches pizza code
	if (!Response.Equals(CurrentOrderCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckOrder has deemed the player messed up the PizzaCode"));
		UE_LOG(LogTemp, Warning, TEXT("Code:  %s"), *CurrentOrderCode);
		UE_LOG(LogTemp, Warning, TEXT("Input: %s"), *Response);
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player successfuly entered code: %s"), *Response);
	}

	UE_LOG(LogTemp, Log, TEXT("Order completed successfully! Giving player %s $%d"),
		*GetName(), CurrentOrder.OrderCost);
	// Add payment to total funds
	Funds += CurrentOrder.OrderCost;

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

void APizzaPlayer::OnBankruptcyMaxed_Implementation() {
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

bool APizzaPlayer::UpdateActionMode(EActionMode NewMode)
{
	ClearSelections();

	// Find preset
	for (const FActionModePreset& Preset : SelectablePresets)
	{
		if (Preset.Mode == NewMode)
		{
			ValidSelectables = Preset.ValidSelectables;
			ActionMode = NewMode;
			return true;
		}
	}

	// No preset found; complain and disble selection
	ValidSelectables.Empty();
	UE_LOG(LogTemp, Error, TEXT("No preset available for %s"),
		*UsefulFunctions::EnumToString(FString("EActionMode"), NewMode));
	return false;
}

void APizzaPlayer::ClearSelections()
{
	// Clear selected nodes
	for (int i = SelectedNodes.Num() - 1; i >= 0; i--)
	{
		SelectedNodes[i]->IsSelected = false;
		SelectedNodes.RemoveAt(i, 1, false); // Don't bother shrinking
	}

	// Clear selected popup
	SelectedPopup->IsSelected = false;
	SelectedPopup = nullptr;
}

void APizzaPlayer::OnSelectedPopupChanged_Implementation()
{
	
}