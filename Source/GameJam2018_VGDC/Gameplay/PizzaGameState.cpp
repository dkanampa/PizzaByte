// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaGameState.h"



APizzaGameState::APizzaGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	PeriodOfDayTimes.Add(EPeriodOfDay::Twilight,  0.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Morning,   6        * 60.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Noon,      12       * 60.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Afternoon, (12 + 4) * 60.0f);
	PeriodOfDayTimes.Add(EPeriodOfDay::Night,     (12 + 8) * 60.0f);
}

void APizzaGameState::BeginPlay()
{
	Super::BeginPlay();

}

void APizzaGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateGameTime(DeltaTime);
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

		UE_LOG(LogTemp, Log, TEXT("The date is now %2d/%2d/%2d (DD/WW/MM)"),
			Day, Week, Month);
		UE_LOG(LogTemp, Log, TEXT("It %s the weekend"),
			IsWeekend() ? TEXT("is") : TEXT("is not"));
	}

	if (Day > WeekDays + WeekendDays)
	{
		Day = 1;
		Week++;

		UE_LOG(LogTemp, Log, TEXT("Week %d of month %d has begun!"), Week, Month);
	}

	if (Week > WeeksPerMonth)
	{
		Week = 1;
		Month++;
		OnNewMonth();

		UE_LOG(LogTemp, Log, TEXT("Welcome to month %d!"), Month);
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

void APizzaGameState::OnNewMonth()
{
	// TODO: Bill player
}