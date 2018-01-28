// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PizzaPlayer.h"
#include "../Environment/FDistrict.h"
#include "FOrder.h"
#include "FCharsetFlags.h"

#include "GameFramework/Actor.h"
#include "PizzaOrderManager.generated.h"

// Forward declare for circular dependencies
class APizzaGameState;
class APizzaPlayer;
class AOrderPopup;

/**
 * Singleton actor responsible for creating orders
 * Spawned by the PizzaGameState
 */
UCLASS()
class GAMEJAM2018_VGDC_API APizzaOrderManager : public AActor
{
	GENERATED_BODY()
	
private:

	float NextOrderGenerationCall = 0.0f;

	int32 OrderSeed = 0; // TODO Make this public UProperty
	int32 PizzaSeed = 0;
	FRandomStream OrderRNG;
	FRandomStream PizzaCodeRNG;

	// Lower case letters; doesn't include null terminator, so don't print!
	TArray<TCHAR> CharSetLowercase;
	// Upper case letters; doesn't include null terminator, so don't print!
	TArray<TCHAR> CharSetUppercase;
	// Numeric characters; doesn't include null terminator, so don't print!
	TArray<TCHAR> CharSetNumeric;
	// Non-alphanumeric characters; doesn't include null terminator, so don't print!
	TArray<TCHAR> CharSetSpecial;
	
	void TestCodeGeneration();

protected:
	virtual void BeginPlay() override;

public:
	APizzaOrderManager();
	virtual void Tick(float DeltaTime) override;

	// Called every hour; may no orders or a bunch, depending on the will of RNGesus
	UFUNCTION(BlueprintCallable, Category = "Orders")
		void GenerateNewOrders();

	// Calculates how likely [0-1] a district will place an order at the
	//   current time
	UFUNCTION(BlueprintCallable, Category = "Orders|Helper Functions")
		float CalculateOrderLikelihood(const FDistrict& District);

	/**
	 * Actually spawns a new order in a district, but doesn't add it to our array
	 * Does so by chosing a sector at random and spawning a pizza in one of its
	 *   blocks, with a topping influenced by the topping preferences
	 */
	UFUNCTION(BlueprintCallable, Category = "Orders")
		FOrder GenerateOrder(const FDistrict& District);

	/**
	 * Generates a randomly long string of garbage whose length depends on
	 *   the distance the code has to travel.
	 * Also different node toppings affect the character set:
	 *   cheese is just [a-z], pepperoni is [a-z][A-Z], sausage is 
	 *   [a-z]/!@#$%^&*(), pineapple is !@#$%^&*()/[0-9]...
	 */
	UFUNCTION(BlueprintCallable, Category = "Pizza Code")
		FString GeneratePizzaCode(float Distance, TArray<APizzaNode*> Nodes);

	/**
	 * Generates our lowercase, uppercase, numeric, and special Character Sets
	 *   for pizza code generation
	 */
	UFUNCTION(BlueprintCallable, Category = "Pizza Code")
		void GenerateCharacterSets();

	/**
	 * Completes an order by removing it from the current
	 * list of open orders.
	 * @param Successful - whether this was completed by a player or it timed out
	 */
	UFUNCTION(BlueprintCallable, Category = "Orders")
		void CompleteOrder(FOrder Order, bool Successful = true);

	// Assigned by GameState when it spawns us
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		class APizzaGameState* GameState;

	// Generated pizza code is Floor(DifficultyMod * Distance) characters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders",
		META = (ClampMin = 0.0f, UIMax = 20.0f))
		float DifficultyModifier = 1.0f;

	// How often (in game minutes) will we call the function that decides
	//   whether to spawn an order
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders",
		META = (ClampMin = 0.0f, UIMax = 1440.0f))
		float OrderGenerationFrequency = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		TArray<FOrder> OpenOrders;

	// Defines order costs
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		TMap<EPizzaTopping, int32> ToppingCosts;

	// Tweaking this can increase/decrease likelihood of order placement.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders",
		META = (ClampMin = 0.0f, UIMax = 2.0f))
		float OrderFrequencyMultiplier = 1.0f;

	// In case a district has no OrderFrequency curve below, fall back on this
	//   one. Note, this should be temporary, as I'll spam some error messages.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		UCurveFloat* OrderFrequencyFallback;

	// How likely it is [0-1] that this type of district will order a pizza at
	//   this time of day, specifically on weekdays
	// Access curve value via GetFloatValue(x)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		TMap<EDistrictType, UCurveFloat*> OrderFrequencyWeekdays;

	// How likely it is [0-1] that this type of district will order a pizza at
	//   this time of day, specifically on weekends
	// Access curve value via GetFloatValue(x)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		TMap<EDistrictType, UCurveFloat*> OrderFrequencyWeekends;

	// Min/Max time in minutes before an order should expire
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		FVector2D ExpireTimeMinMax = FVector2D(30.0f, 120.0f);

	// All players in the game; automatically queried for at the start of the 
	//   game
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Environment")
		TArray<APizzaPlayer*> Players;

	// All districts in the map; should be provided externally
	// REMEMBER FOR GOD'S SAKE TO PASS BY REFERENCE!!!!!!!!!!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Environment")
		TArray<FDistrict> Districts;

	// Map of what set of characters a node of the given topping will
	//   add to the generation set
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pizza Code")
		TMap<EPizzaTopping, FCharsetFlags> ToppingCodeCharacterSets;

};
