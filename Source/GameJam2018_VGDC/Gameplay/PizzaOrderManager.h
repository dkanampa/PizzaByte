// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PizzaPlayer.h"
#include "../Environment/FDistrict.h"
#include "FOrder.h"

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "PizzaOrderManager.generated.h"

/**
 * Singleton actor responsible for creating orders
 * Spawned by the PizzaGameState
 */
UCLASS()
class GAMEJAM2018_VGDC_API APizzaOrderManager : public AActor
{
	GENERATED_BODY()
	
private:

	float LastOrderGenerationCall = 0.0f;

	int32 Seed = 0; // TODO Make this public UProperty
	FRandomStream RNG;
	
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

	// Assigned by GameState when it spawns us
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		class APizzaGameState* GameState;

	// How often (in game minutes) will we call the function that decides
	//   whether to spawn an order
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders",
		META = (ClampMin = 0.0f, UIMax = 1440.0f))
		float OrderGenerationFrequency = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		TArray<FOrder> OpenOrders;

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

};
