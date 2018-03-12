// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Environment/FDistrict.h"
#include "Environment/FSector.h"
#include "PizzaNode.h"
#include "FOrder.h"
#include "OrderPopup.h"
#include "../Environment/FSector.h"
#include "../Environment/FBlock.h"
#include "PizzaPlayer.generated.h"

class APizzaGameState;
class APizzaOrderManager;
class ALevelManager;
class APizzaController;

UENUM(BlueprintType)
enum class EActionMode : uint8
{
	Disabled,
	OrderSelection,
	OrderChaining,
	Placing,
	Selling
};

/**
 * For neatly and extensibly providing selection presets for action modes
 */
USTRUCT(BlueprintType)
struct FActionModePreset
{
	GENERATED_USTRUCT_BODY();

	FActionModePreset() {};

	FActionModePreset(EActionMode _Mode) : Mode(_Mode) {};

	FActionModePreset(EActionMode _Mode,
		TArray<TSubclassOf<AActor>> _ValidSelectables)
		: Mode(_Mode), ValidSelectables(_ValidSelectables) {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EActionMode Mode;

	// List of things we can or can not select in the given ActionMode
	// Selecting someting not in this list will be ignored
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<TSubclassOf<AActor>> ValidSelectables;
};

UCLASS()
class GAMEJAM2018_VGDC_API APizzaPlayer : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	bool AlreadyBankrupt = false;

	bool AssertHasGameState();

public:
	APizzaPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Input")
		void StartSelect();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void StopSelect();

	bool PurchaseTowerInSector(FSector* Sector, FBlock* ToPurchase, EPizzaTopping Topping);

	bool PurchaseDistrictPermit(FDistrict* District);

	UFUNCTION(BlueprintCallable, Category = "Orders")
		bool CheckOrder(FString Response);

	bool hasBoughtSectorPermit(FDistrict* District);

	bool PurchaseTowerWithFunds(FBlock* ToPurchase);

	int32 GetOwnedNodesSizeInDistrict(FDistrict* District);

	/**
	 * Sets the action mode, clearing selections et al.
	 * @return Whether changing new action mode was successful (i.e. presets found)
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
		bool UpdateActionMode(EActionMode NewMode);

	/**
	 * In case we do funky stuff with selections and we want to keep it neat
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
		void ClearSelections();

	/**
	 * Makes sure that the path to the order is valid:
	 *   All necessary toppings are included,
	 *   Distance between last node and order isn't to ofar
	 * @param ReturnedPathLength returns by reference the calculated distance for the entire path
	 */
	UFUNCTION(BlueprintCallable)
		bool IsValidPath(FOrder Order, TArray<APizzaNode*> Path, float& ReturnedPathLength);

	/**
	 * Can take negative or positive values.
	 * Notifies the GameState when bankruptcy has begun/finished so it can
	 *   time us accordingly
	 */
	UFUNCTION(BlueprintCallable)
		void AddOrRemoveFunds(int32 Amount);

	UFUNCTION(BlueprintCallable)
		int32 GetFunds();

	/**
	 * Called once our funds are negative for more than MaxBankruptcyTime
	 *   minutes
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Management")
		void OnBankruptcyMaxed();

	// Assigned on BeginPlay
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "References")
		APizzaController* PizzaController;

	// Assigned by GameState immediately after begin play
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "References")
		APizzaGameState* GameState;

	// Assigned by GameState immediately after begin play
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "References")
		APizzaOrderManager* OrderManager;

	// Assigned by GameState immediately after begin play
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "References")
		ALevelManager* LevelManager;

	// Maximum time (in-game minutes) the player can be bankrupt before loss
	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		META = (ClampMin = 0.0f, UIMax = 5760.0f))
		float MaxBankruptcyTime = 2880.0f;

	// Do not change manually! Use add/remove funds!
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		int32 Funds;

	// Read only on purpose; use functions to add/remove
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		TArray<APizzaNode*> Nodes;

	// Read only on purpose; use functions to add/remove
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		TArray<FDistrict> PermittedDistricts;

	// Whether we're selecting node's for an order, buying nodes, etc.
	// Set by UpdateActionMode
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Input")
		EActionMode ActionMode = EActionMode::Disabled;

	// List of things we can or can not select in our current ActionMode
	// Selecting someting not listed here will be ignored
	// Set by UpdateActionMode
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Input")
		TArray<TSubclassOf<AActor>> ValidSelectables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		TArray<FActionModePreset> SelectablePresets;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		TArray<APizzaNode*> SelectedNodes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		AOrderPopup* SelectedPopup;

	// Max distance between last node and an order that we'll allow
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		float MaxNodeOrderDistance = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		FOrder CurrentOrder;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		FString CurrentOrderCode = "";
};
