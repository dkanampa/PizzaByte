// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaOrderManager.h"
#include "PizzaGameState.h"
#include "PizzaPlayer.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "../GenericUsefulFunctions.h"


APizzaOrderManager::APizzaOrderManager()
{
	PrimaryActorTick.bCanEverTick = true;

	ToppingCosts.Add(EPizzaTopping::Cheese,    10);
	ToppingCosts.Add(EPizzaTopping::Pepperoni, 12);
	ToppingCosts.Add(EPizzaTopping::Sausage,   12);
	ToppingCosts.Add(EPizzaTopping::Pineapple, 14);
}

void APizzaOrderManager::BeginPlay()
{
	Super::BeginPlay();

	OrderRNG = FRandomStream(OrderSeed);
	PizzaCodeRNG = FRandomStream(PizzaSeed);

	OpenOrders.Reserve(32);

	for (int i = 0; i < 8; i++)
	{
		GeneratePizzaCode(FMath::FRandRange(10.0f, 32.0f));
	}
}

void APizzaOrderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrderManager has no APizzaGameState! Searching..."));
		GameState = Cast<APizzaGameState>(GetWorld()->GetGameState());

		if (GameState == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("OrderManager relies on the APizzaGameState, but none found!"));
			return;
		}
	}

	if (GameState->TimeOfDay >= NextOrderGenerationCall && 
		GameState->TimeOfDay + OrderGenerationFrequency < 1440.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Recalculating orders..."));
		GenerateNewOrders();
		NextOrderGenerationCall = GameState->TimeOfDay + OrderGenerationFrequency;
		if (NextOrderGenerationCall >= 1440.0f)
			NextOrderGenerationCall -= 1440.0f;
	}

	// Expire old orders
	for (int i = 0; i < OpenOrders.Num(); i++)
	{
		if (OpenOrders[i].ExpireTime < GameState->TimeOfDay)
		{
			UE_LOG(LogTemp, Log, TEXT("Expiring Order for %s pizza"),
				*UsefulFunctions::EnumToString(FString("EPizzaTopping"), OpenOrders[i].PizzaType));

			OpenOrders.RemoveAt(i, 1, false); // i.e. don't shrink
			i--;
		}
	}
		
}

void APizzaOrderManager::GenerateNewOrders()
{
	// TODO: Only query districts that player's have towers in
	for (FDistrict& District : Districts)
	{
		float OrderLikelihood = CalculateOrderLikelihood(District);

		if (OrderRNG.FRand() < OrderLikelihood)
		{
			UE_LOG(LogTemp, Log, TEXT("Placing order in District %s..."), 
				*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type));

			OpenOrders.Add(GenerateOrder(District));
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
	float RandomNumber = OrderRNG.FRand();
	float PreviousChancesSum = 0.0f;
	for (auto& Elem : District.ToppingPreferences)
	{
		if (RandomNumber < PreviousChancesSum + Elem.Value)
			ChosenTopping = Elem.Key;
		else
			PreviousChancesSum += Elem.Value;
	}

	/** Assign cost based on pizza topping */
	int32 OrderCost = 10;
	if(ToppingCosts.Find(ChosenTopping) != nullptr)
		OrderCost = *ToppingCosts.Find(ChosenTopping);

	/** Choose a sector and block */
	FSector ChosenSector = UsefulFunctions::RandomElementInArray(District.Sectors, &OrderRNG);
	FBlock ChosenBlock = UsefulFunctions::RandomElementInArray(ChosenSector.Blocks, &OrderRNG);

	/** Choose Expirey Time */
	float ExpireDuration = OrderRNG.FRandRange(ExpireTimeMinMax.X, ExpireTimeMinMax.Y);

	float ExpireTime = GameState->TimeOfDay + ExpireDuration;
	if (ExpireTime >= 1440.0f)
		ExpireTime -= 1440.0f;

	UE_LOG(LogTemp, Log, TEXT("Spawning an order for a %s Pizza for $%d in district %s, expiring in %.0f minutes (at time %.2f)"), 
		*UsefulFunctions::EnumToString(FString("EPizzaTopping"), ChosenTopping), OrderCost,
		*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type),
		ExpireDuration, ExpireTime);

	return FOrder(ChosenTopping, OrderCost, ChosenBlock, GameState->TimeOfDay, ExpireTime);
}

FString APizzaOrderManager::GeneratePizzaCode(float Distance)
{
	int32 StringLength = FMath::FloorToInt(Distance * DifficultyModifier);
	
	// TCHAR seems to be ASCII - 32 (skipping extra crap before actual text characters)
	// http://en.cppreference.com/w/cpp/language/ascii
	TArray<TCHAR> GarbledString;
	GarbledString.Init(' ', StringLength + 1);
	GarbledString[StringLength] = '\0';

	for (int i = 0; i < StringLength; i++)
	{
		GarbledString[i] = ' ' + (PizzaCodeRNG.RandRange(97, 122) - 32);
	}	

	FString Output = GarbledString.GetData();
	
	UE_LOG(LogTemp, Log, TEXT("Generating random string: %s"),
		*Output);
	
	return Output;
}