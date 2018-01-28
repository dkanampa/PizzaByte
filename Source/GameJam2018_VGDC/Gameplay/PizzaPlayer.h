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

UCLASS()
class GAMEJAM2018_VGDC_API APizzaPlayer : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	int32 Funds = 0;

	bool AlreadyBankrupt = false;

	bool AssertHasGameState();

public:
	APizzaPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool PurchaseTowerInSector(FSector* Sector, FBlock* ToPurchase, EPizzaTopping Topping);

	bool PurchaseDistrictPermit(FDistrict* District);

	UFUNCTION(BlueprintCallable, Category = "Orders")
		bool CheckOrder(TArray<APizzaNode*> PizzaNodes, FString response);

	bool hasBoughtSectorPermit(FDistrict* District);

	bool PurchaseTowerWithFunds(FBlock* ToPurchase);

	int32 GetOwnedNodesSizeInDistrict(FDistrict* District);

	UFUNCTION(BlueprintCallable)
		bool IsValidPath(EPizzaTopping Topping, TArray<APizzaNode*> Path);

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
	UFUNCTION(BlueprintCallable)
		void OnBankruptcyMaxed();

	// Assigned by GameState immediately after begin play
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		APizzaGameState* GameState;

	// Assigned by GameState immediately after begin play
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		APizzaOrderManager* OrderManager;

	// Assigned by GameState immediately after begin play
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		ALevelManager* LevelManager;

	// Maximum time (in-game minutes) the player can be bankrupt before loss
	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		META = (ClampMin = 0.0f, UIMax = 5760.0f))
		float MaxBankruptcyTime = 2880.0f;
	
	// FUNDS ARE PRIVATE, USE ADD/REMOVEFUNDS AND GETFUNDS
	// This is only here in case UI absolutely needs it for some reason
	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//	int32 Funds_READONLY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<APizzaNode*> Nodes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDistrict> PermittedDistricts;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<APizzaNode*> SelectedNodes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AOrderPopup* SelectedPopup;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FOrder CurrentOrder;
};