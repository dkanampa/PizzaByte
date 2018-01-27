// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaGameState.h"
#include "../GenericUsefulFunctions.h"


APizzaGameState::APizzaGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	PeriodOfDayTimes.Add(EPeriodOfDay::Twilight,  0.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Morning,   6        * 60.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Noon,      12       * 60.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Afternoon, (12 + 4) * 60.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Night,     (12 + 8) * 60.0f);

	Seasons.Add(ESeason::Spring, 0);
	Seasons.Add(ESeason::Summer, 2);
	Seasons.Add(ESeason::Fall,   4);
	Seasons.Add(ESeason::Winter, 6);
}

void APizzaGameState::BeginPlay()
{
	Super::BeginPlay();

	RNG = FRandomStream(Seed);
}

void APizzaGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateGameTime(DeltaTime);

	if (LastOrderGenerationCall + OrderGenerationFrequency <= TimeOfDay)
	{
		UE_LOG(LogTemp, Log, TEXT("Recalculating orders..."));
		GenerateNewOrders();
		LastOrderGenerationCall = TimeOfDay;
	}
		
}

void APizzaGameState::UpdateGameTime(float DeltaTime)
{
	TimeOfDay += DeltaTime * TimeSpeed;

	if (TickPreviousPeriodOfDay != GetPeriodOfDay())
	{
		UE_LOG(LogTemp, Log, TEXT("Time: %f"), TimeOfDay);
		TickPreviousPeriodOfDay = GetPeriodOfDay();
	}		

	if (TimeOfDay >= 1440.0f)
	{
		TimeOfDay = 1440.0f - TimeOfDay;
		Day++;

		UE_LOG(LogTemp, Log, TEXT("The date is now %s"),
			*GetTimestamp(false));
		UE_LOG(LogTemp, Log, TEXT("It %s the weekend"),
			IsWeekend() ? TEXT("is") : TEXT("is not"));
	}

	if (Day > WeekDays + WeekendDays)
	{
		Day = 1;
		Week++;

		UE_LOG(LogTemp, Log, TEXT("Week %d of month %d has begun!"), Week, Month);

		if (TickPreviousSeason != GetSeason())
		{
			UE_LOG(LogTemp, Log, TEXT("Welcome to a new season: %s!"), 
				*UsefulFunctions::EnumToString(FString("ESEason"), GetSeason()));
			TickPreviousSeason = GetSeason();
		}
	}

	if (Week > WeeksPerMonth)
	{
		Week = 1;
		Month++;
		OnNewMonth();

		UE_LOG(LogTemp, Log, TEXT("Welcome to month %d!"), Month);
	}

	if (Month > MonthsPerYear)
	{
		Month = 1;
		Year++;

		UE_LOG(LogTemp, Log, TEXT("Happy new year, %d!"), Year);
	}
}

bool APizzaGameState::IsWeekend()
{
	return Day > WeekDays;
}

EPeriodOfDay APizzaGameState::GetPeriodOfDay()
{
	// Assumes PeriodOfDayTimes is sorted

	EPeriodOfDay PrevPeriod = EPeriodOfDay::Twilight;
	for (auto& Elem : PeriodOfDayTimes)
	{
		if (TimeOfDay < Elem.Value)
			return PrevPeriod;
		else
			PrevPeriod = Elem.Key;
	}

	// Edge Case: We reached the last period, meaning above for loop won't
	//   return
	return PrevPeriod;
}

ESeason APizzaGameState::GetSeason()
{
	// Assumes Seasons map is sorted

	ESeason PrevSeason = ESeason::Spring;
	for (auto& Elem : Seasons)
	{
		if (Month < Elem.Value)
			return PrevSeason;
		else
			PrevSeason = Elem.Key;
	}

	// Edge Case: We reached the last period, meaning above for loop won't
	//   return
	return PrevSeason;
}

void APizzaGameState::OnNewMonth()
{
	// TODO: Bill player
}

FString APizzaGameState::GetTimestamp(bool IncludeTimeOfDay)
{
	if(IncludeTimeOfDay)
		return FString::Printf(TEXT("%07.2f:%02d/%02d/%02d/%04d"), TimeOfDay, Day, Week, Month, Year);
	else
		return FString::Printf(TEXT("%02d/%02d/%02d/%04d"), Day, Week, Month, Year);
}

void APizzaGameState::GenerateNewOrders()
{
	for (FDistrict& District : Districts)
	{
		float OrderLikelihood = CalculateOrderLikelihood(District);

		if (RNG.FRand() < OrderLikelihood)
		{
			UE_LOG(LogTemp, Log, TEXT("Placing order in District %s..."), 
				*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type));
		}
	}
}

float APizzaGameState::CalculateOrderLikelihood(const FDistrict& District)
{
	/** Get District Frequency Curve for the current day */
	UCurveFloat* FreqCurve = nullptr;

	if (IsWeekend())
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
		UE_LOG(LogTemp, Error, TEXT("IsWeekend: %s"), IsWeekend() ? TEXT("true") : TEXT("false"));

		if (OrderFrequencyFallback == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Bollocks! We can't even use a fallback because you didn't provide it!"));
			UE_LOG(LogTemp, Error, TEXT("Skipping District!"));
			return -1.0f;
		}
	}

	/** Calculate Likelihood */
	float OrderLikelihood = OrderFrequencyMultiplier * FreqCurve->GetFloatValue(TimeOfDay);

	UE_LOG(LogTemp, Log, TEXT("Likelihood of an order being placed in a %s district at %s: %.4f"),
		*UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type),
		*GetTimestamp(), OrderLikelihood);
	UE_LOG(LogTemp, Log, TEXT("\t(calculation: FreqMod (%.4f) * Curve (%.4f), given curve %s)"),
		OrderFrequencyMultiplier, FreqCurve->GetFloatValue(TimeOfDay), *FreqCurve->GetName());

	return OrderLikelihood;
}