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

UENUM(BlueprintType)
enum class EActionMode : uint8
{
	Disabled,
	OrderChaining,
	Placing,
	Selling
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

	bool PurchaseTowerInSector(FSector* Sector, FBlock* ToPurchase, EPizzaTopping Topping);

	bool PurchaseDistrictPermit(FDistrict* District);

	UFUNCTION(BlueprintCallable, Category = "Orders")
		bool CheckOrder(FString Response);

	bool hasBoughtSectorPermit(FDistrict* District);

	bool PurchaseTowerWithFunds(FBlock* ToPurchase);

	int32 GetOwnedNodesSizeInDistrict(FDistrict* District);

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		EActionMode ActionMode = EActionMode::Disabled;

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
