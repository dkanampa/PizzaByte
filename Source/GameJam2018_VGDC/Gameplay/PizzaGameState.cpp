// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaGameState.h"



APizzaGameState::APizzaGameState()
{
	PrimaryActorTick.bCanEverTick = true;

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

	UE_LOG(LogTemp, Log, TEXT("The time is now %.2f"), TimeOfDay);

	if (TimeOfDay >= 1440.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("The date is now %2d/%2d/%2d (DD/WW/MM)"), 
			Day, Week, Month);
		UE_LOG(LogTemp, Log, TEXT("It %s the weekend"), 
			IsWeekend() ? TEXT("is") : TEXT("is not"));
		TimeOfDay = 1440.0f - TimeOfDay;
		Day++;
	}

	if (Day > WeekDays + WeekendDays)
	{
		UE_LOG(LogTemp, Log, TEXT("Week %d of month %d has begun!"), Week, Month);
		Day = 1;
		Week++;
	}

	if (Week > WeeksPerMonth)
	{
		UE_LOG(LogTemp, Log, TEXT("Welcome to month %d!"), Month);
		Week = 1;
		Month++;
		OnNewMonth();
	}
}

bool APizzaGameState::IsWeekend()
{
	return Day > WeekDays;
}

void APizzaGameState::OnNewMonth()
{
	// TODO: Bill player
}