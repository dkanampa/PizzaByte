// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaOrderManager.h"
#include "PizzaGameState.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "../GenericUsefulFunctions.h"


APizzaOrderManager::APizzaOrderManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APizzaOrderManager::BeginPlay()
{
	Super::BeginPlay();

	RNG = FRandomStream(Seed);
}

void APizzaOrderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrderManager has no APizzaGameState! Searching..."));
		GameState = Cast<APizzaGameState>(GetWorld()->GetGameState());

		if (!GameState)
		{
			UE_LOG(LogTemp, Error, TEXT("OrderManager relies on the APizzaGameState, but none found!"));
			return;
		}
	}

	if (LastOrderGenerationCall + OrderGenerationFrequency <= GameState->TimeOfDay)
	{
		UE_LOG(LogTemp, Log, TEXT("Recalculating orders..."));
		GenerateNewOrders();
		LastOrderGenerationCall = GameState->TimeOfDay;
	}
		
}

void APizzaOrderManager::GenerateNewOrders()
{
	// TODO: Only query districts that player's have towers in
	for (FDistrict& District : Districts)
	{
		float OrderLikelihood = CalculateOrderLikelihood(District);

		if (RNG.FRand() < OrderLikelihood)
		{
			UE_LOG(LogTemp, Log, TEXT("Placing order in District %s..."), 
				*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type));


			// TODO: The array ends up having "None" elements, so I suspect the game state
			//   is messing with things. Move this to an actor.
			FOrder SpawnedOrder = GenerateOrder(District);

			UE_LOG(LogTemp, Log, TEXT("Adding order (%d) to array..."),
				(uint8)SpawnedOrder.PizzaType);

			OpenOrders.Add(SpawnedOrder);
		}
	}
}

float APizzaOrderManager::CalculateOrderLikelihood(const FDistrict& District)
{
	/** Get District Frequency Curve for the current day */
	UCurveFloat* FreqCurve = nullptr;

	if (GameState->IsWeekend())
	{
		if (OrderFrequencyWeekends.Find(District.Type) != nullptr)
		{
			FreqCurve = *OrderFrequencyWeekends.Find(District.Type);
		}
	}
	else
	{
		if (OrderFrequencyWeekends.Find(District.Type) != nullptr)
		{
			FreqCurve = *OrderFrequencyWeekends.Find(District.Type);
		}
	}

	if (FreqCurve == nullptr)
	{
		FreqCurve = OrderFrequencyFallback;

		UE_LOG(LogTemp, Error, TEXT("District Type %s does not have a order frequency! Using Fallback."),
			*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type));
		UE_LOG(LogTemp, Error, TEXT("IsWeekend: %s"), GameState->IsWeekend() ? TEXT("true") : TEXT("false"));

		if (OrderFrequencyFallback == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Bollocks! We can't even use a fallback because you didn't provide it!"));
			UE_LOG(LogTemp, Error, TEXT("Skipping District!"));
			return -1.0f;
		}
	}

	/** Calculate Likelihood */
	float OrderLikelihood = OrderFrequencyMultiplier * FreqCurve->GetFloatValue(GameState->TimeOfDay);

	UE_LOG(LogTemp, Log, TEXT("Likelihood of an order being placed in a %s district at %s: %.4f"),
		*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type),
		*GameState->GetTimestamp(), OrderLikelihood);
	UE_LOG(LogTemp, Log, TEXT("\t(calculation: FreqMod (%.4f) * Curve (%.4f), given curve %s)"),
		OrderFrequencyMultiplier, FreqCurve->GetFloatValue(GameState->TimeOfDay), *FreqCurve->GetName());

	return OrderLikelihood;
}

FOrder APizzaOrderManager::GenerateOrder(const FDistrict& District)
{
	/** Choose a topping */
	EPizzaTopping ChosenTopping;

	// Chooses the topping using the district's likelihoods, assuming they add
	//   up to 1.0
	// May or may not work; no time to explain, it's a game jam!
	float RandomNumber = RNG.FRand();
	float PreviousChancesSum = 0.0f;
	for (auto& Elem : District.ToppingPreferences)
	{
		if (RandomNumber < PreviousChancesSum + Elem.Value)
			ChosenTopping = Elem.Key;
		else
			PreviousChancesSum += Elem.Value;
	}

	/** Choose a sector and block */
	FSector ChosenSector = UsefulFunctions::RandomElementInArray(District.Sectors, &RNG);
	FBlock ChosenBlock = UsefulFunctions::RandomElementInArray(ChosenSector.Blocks, &RNG);

	/** Choose Expirey Time */
	float ExpireDuration = RNG.FRandRange(ExpireTimeMinMax.X, ExpireTimeMinMax.Y);

	UE_LOG(LogTemp, Log, TEXT("Spawning an order for a %s Pizza in district %s, expiring in %.0f minutes (at time %7.2f)"), 
		*UsefulFunctions::EnumToString(FString("EPizzaTopping"), ChosenTopping),
		*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type),
		ExpireDuration, GameState->TimeOfDay + ExpireDuration);

	return FOrder(ChosenTopping, ChosenBlock, GameState->TimeOfDay, GameState->TimeOfDay + ExpireDuration);
}