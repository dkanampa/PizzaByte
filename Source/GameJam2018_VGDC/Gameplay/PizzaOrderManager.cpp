// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaOrderManager.h"
#include "PizzaGameState.h"
#include "PizzaPlayer.h"
#include "OrderPopup.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "../GenericUsefulFunctions.h"


APizzaOrderManager::APizzaOrderManager()
{
	PrimaryActorTick.bCanEverTick = true;

	ToppingCosts.Add(EPizzaTopping::Cheese,    10);
	ToppingCosts.Add(EPizzaTopping::Pepperoni, 12);
	ToppingCosts.Add(EPizzaTopping::Sausage,   12);
	ToppingCosts.Add(EPizzaTopping::Pineapple, 14);

	ToppingCodeCharacterSets.Add(EPizzaTopping::Cheese,    FCharsetFlags(true,  false, false, false));
	ToppingCodeCharacterSets.Add(EPizzaTopping::Pepperoni, FCharsetFlags(true,  true,  false, false));
	ToppingCodeCharacterSets.Add(EPizzaTopping::Sausage,   FCharsetFlags(true,  true,  true,  false));
	ToppingCodeCharacterSets.Add(EPizzaTopping::Pineapple, FCharsetFlags(true,  false, true,  true));

	OrderPopupClass = AOrderPopup::StaticClass();
}

void APizzaOrderManager::BeginPlay()
{
	Super::BeginPlay();

	GenerateCharacterSets();

	OrderRNG = FRandomStream(FMath::RandRange(0, 100));
	PizzaCodeRNG = FRandomStream(FMath::RandRange(0, 100));

	OpenOrders.Reserve(32);

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

	NextOrderDeltaTime -= GameState->GetScaledTime(DeltaTime);

	if (NextOrderDeltaTime <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Recalculating orders..."));
		GenerateNewOrders();
		NextOrderDeltaTime = OrderGenerationFrequency;
	}

	// Expire old orders
	for (int i = 0; i < OpenOrders.Num(); i++)
	{
		if (OpenOrders[i] == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't expire nullptr"));
			
			OpenOrders.RemoveAt(i, 1, false); // i.e. don't shrink
			i--;
		}
			

		if (OpenOrders[i]->Order.ExpireTime < GameState->TimeOfDay)
		{
			UE_LOG(LogTemp, Log, TEXT("Expiring order %s"), *OpenOrders[i]->GetName());

			CompleteOrder(OpenOrders[i], false);
		}
	}
		
}

void APizzaOrderManager::GenerateNewOrders()
{
	if (Districts.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("GenerateOrders called, but districts array is empty! "
			"Orders cannot be generated without districts to be spawned in."));
		return;
	}

	// TODO: Only query districts that player's have towers in
	for (FDistrict& District : Districts)
	{
		float OrderLikelihood = CalculateOrderLikelihood(District);

		if (OrderRNG.FRand() < OrderLikelihood)
		{
			UE_LOG(LogTemp, Log, TEXT("Placing order in District %s..."), 
				*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type));

			AOrderPopup* Popup = GetWorld()->SpawnActor<AOrderPopup>(OrderPopupClass);
			Popup->Order = GenerateOrder(District);
			Popup->SetActorLocation(Popup->Order.Block.GetBlockCenter());
			OpenOrders.Add(Popup);
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
		{
			ChosenTopping = Elem.Key;
			break;
		}
		else
		{
			PreviousChancesSum += Elem.Value;
		}
			
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

void APizzaOrderManager::CompleteOrder(AOrderPopup* Order, bool Successful)
{
	OpenOrders.Remove(Order);
	Order->Destroy();
}

void APizzaOrderManager::GenerateCharacterSets()
{
	/*
	TArray<TCHAR> Everything;
	Everything.Init(' ', 256);
	Everything[255] = '\0';

	for (int i = 0; i < 255; i++)
	{
	Everything[i] = ' ' + i;
	}

	FString EverythingStr = Everything.GetData();

	UE_LOG(LogTemp, Log, TEXT("00        10        20        30        40        50        60        70        80        90        "));
	UE_LOG(LogTemp, Log, TEXT("0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789"));
	UE_LOG(LogTemp, Log, TEXT("%s"), *EverythingStr);
	*/

	// All characters are offset by ' '
	// a-z: [65, 90]
	// A-Z: [33, 58]
	// 0-9: [16, 25]
	// Special: [1, 15] + [26, 32] + [59, 64] + [91, 94]

	// a-z: ' ' + [65, 90]
	CharSetLowercase.Init(' ', 90 - 65 + 1);

	for (int i = 0; i < 90 - 65 + 1; i++)
		CharSetLowercase[i] = ' ' + i + 65;


	// A-Z: ' ' + [33, 58]
	CharSetUppercase.Init(' ', 58 - 33 + 1);

	for (int i = 0; i < 58 - 33 + 1; i++)
		CharSetUppercase[i] = ' ' + i + 33;


	// 0-9: ' ' + [16, 25]
	CharSetNumeric.Init(' ', 25 - 16 + 1);

	for (int i = 0; i < 25 - 16 + 1; i++)
		CharSetNumeric[i] = ' ' + i + 16;


	// Special: ' ' + ([1, 15], [26, 32], [59, 64], [91, 94])
	CharSetSpecial.Init(' ', (15 - 1) + (32 - 26) + (64 - 59) + (94 - 91) + 4);
	int32 SpecialIdx = 0;

	for (int i = 0; i < 15 - 1 + 1; i++)
		CharSetSpecial[SpecialIdx++] = ' ' + i + 1;

	for (int i = 0; i < 32 - 26 + 1; i++)
		CharSetSpecial[SpecialIdx++] = ' ' + i + 26;

	for (int i = 0; i < 64 - 59 + 1; i++)
		CharSetSpecial[SpecialIdx++] = ' ' + i + 59;

	for (int i = 0; i < 94 - 91 + 1; i++)
		CharSetSpecial[SpecialIdx++] = ' ' + i + 91;

	/*
	CharSetLowercase.Add('\0');
	FString Lowercase = CharSetLowercase.GetData();
	UE_LOG(LogTemp, Log, TEXT("Lowercase: (%s)"), *Lowercase);

	CharSetUppercase.Add('\0');
	FString Uppercase = CharSetUppercase.GetData();
	UE_LOG(LogTemp, Log, TEXT("Uppercase: (%s)"), *Uppercase);

	CharSetNumeric.Add('\0');
	FString Numeric = CharSetNumeric.GetData();
	UE_LOG(LogTemp, Log, TEXT("Numeric: (%s)"), *Numeric);

	CharSetSpecial.Add('\0');
	FString Special = CharSetSpecial.GetData();
	UE_LOG(LogTemp, Log, TEXT("Special: (%s)"), *Special);
	*/
}

FString APizzaOrderManager::GeneratePizzaCode(float Distance, TArray<APizzaNode*> Nodes)
{
	int32 StringLength = FMath::Clamp(FMath::FloorToInt( (Distance / 100.0f) * DifficultyModifier), 4, 32);
	
	// TODO: Learn Bitmasks so I don't have to do this
	FCharsetFlags SumFlags;
	for (APizzaNode* Node : Nodes)
	{
		FCharsetFlags* CharacterSets = ToppingCodeCharacterSets.Find(Node->Topping);
		if (CharacterSets != nullptr)
		{
			SumFlags.Lowercase = SumFlags.Lowercase || CharacterSets->Lowercase;
			SumFlags.Uppercase = SumFlags.Uppercase || CharacterSets->Uppercase;
			SumFlags.Numbers = SumFlags.Numbers || CharacterSets->Numbers;
			SumFlags.Special = SumFlags.Special || CharacterSets->Special;
		}
	}

	TArray<TCHAR> CharacterSet;

	FString UsedSets = "";

	if (SumFlags.Lowercase) {
		CharacterSet.Append(CharSetLowercase);
		UsedSets.Append("Lowercase, ");
	}

	if (SumFlags.Uppercase) {
		CharacterSet.Append(CharSetUppercase);
		UsedSets.Append("Uppercase, ");
	}

	if (SumFlags.Numbers) {
		CharacterSet.Append(CharSetNumeric);
		UsedSets.Append("Numbers, ");
	}

	if (SumFlags.Special) {
		CharacterSet.Append(CharSetSpecial);
		UsedSets.Append("Special");
	}

	UE_LOG(LogTemp, Log, TEXT("Generating random string %d characters long using the following character sets: %s"),
		StringLength, *UsedSets);

	TArray<TCHAR> GarbledString;
	GarbledString.Init(' ', StringLength + 1);
	GarbledString[StringLength] = '\0';


	for (int i = 0; i < StringLength; i++)
	{
		GarbledString[i] = UsefulFunctions::RandomElementInArray(CharacterSet, &PizzaCodeRNG);
	}	

	FString Output = GarbledString.GetData();
	
	UE_LOG(LogTemp, Log, TEXT("Generated string: %s"),
		*Output);
	
	return Output;
}

//void APizzaOrderManager::TestCodeGeneration()
//{
//	APizzaNode* Cheese = GetWorld()->SpawnActor<APizzaNode>();
//	Cheese->Topping = EPizzaTopping::Cheese;
//	APizzaNode* Pepperoni = GetWorld()->SpawnActor<APizzaNode>();
//	Pepperoni->Topping = EPizzaTopping::Pepperoni;
//	APizzaNode* Sausage = GetWorld()->SpawnActor<APizzaNode>();
//	Sausage->Topping = EPizzaTopping::Sausage;
//	APizzaNode* Pineapple = GetWorld()->SpawnActor<APizzaNode>();
//	Pineapple->Topping = EPizzaTopping::Pineapple;
//
//	for (int i = 0; i < 8; i++)
//	{
//		TArray<APizzaNode*> Nodes;
//		Nodes.Add(Cheese);
//
//		if (FMath::RandBool())
//			Nodes.Add(Pepperoni);
//		if (FMath::RandBool())
//			Nodes.Add(Sausage);
//		if (FMath::RandBool())
//			Nodes.Add(Pineapple);
//
//		GeneratePizzaCode(FMath::FRandRange(10.0f, 32.0f), Nodes);
//	}
//}
