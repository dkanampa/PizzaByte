// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaGameState.h"
#include "PizzaOrderManager.h"
#include "PizzaPlayer.h"
#include "../Environment/LevelManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
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

	OrderManagerClass = APizzaOrderManager::StaticClass();
}

void APizzaGameState::BeginPlay()
{
	Super::BeginPlay();


	FindOrSpawnOrderManager();
	FindOrSpawnLevelManager();
	
}

void APizzaGameState::PostBeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("Calling PostBeginPlay..."));

	HasHadFirstTick = true;

	/** Assign Player Values */
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APizzaPlayer::StaticClass(), PlayerActors);

	for(AActor* PlayerActor : PlayerActors)
	{
		APizzaPlayer* Player = Cast<APizzaPlayer>(PlayerActor);
		Player->GameState = this;
		Player->OrderManager = OrderManager;
		Player->LevelManager = LevelManager;

		Players.Add(Player);

		UE_LOG(LogTemp, Log, TEXT("GameState added & initialized Player %s"), *Player->GetName());
	}
}

void APizzaGameState::FindOrSpawnOrderManager()
{
	TArray<AActor*> OrderManagerCandidates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APizzaOrderManager::StaticClass(), OrderManagerCandidates);

	if (OrderManagerCandidates.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No OrderManagers found in level by GameState. That's good; we'll spawn our own."));

		if (OrderManagerClass != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawning from class %s"), *OrderManagerClass->GetName());
			OrderManager = GetWorld()->SpawnActor<APizzaOrderManager>(OrderManagerClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("OrderManagerClass not provided; falling back on default class."));
			OrderManager = GetWorld()->SpawnActor<APizzaOrderManager>();
		}
			UE_LOG(LogTemp, Error, TEXT(""));
	}
	else if (OrderManagerCandidates.Num() == 1)
	{
		UE_LOG(LogTemp, Log, TEXT("OrderManager found by GameState in level already. "));

		OrderManager = Cast<APizzaOrderManager>(OrderManagerCandidates[0]);
	}
	else
	{
		OrderManager = Cast<APizzaOrderManager>(OrderManagerCandidates[0]);

		UE_LOG(LogTemp, Warning, TEXT("More than one OrderManagers found by GameState. This will cause undefined behaviour. Selecting first (%s)"),
			*OrderManager->GetName());
	}

	OrderManager->GameState = this;
}

void APizzaGameState::FindOrSpawnLevelManager()
{
	TArray<AActor*> LevelManagerCandidates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelManager::StaticClass(), LevelManagerCandidates);

	if (LevelManagerCandidates.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No LevelManagers found in level by GameState. That's good; we'll spawn our own."));

		LevelManager = GetWorld()->SpawnActor<ALevelManager>();
	}
	else if (LevelManagerCandidates.Num() == 1)
	{
		UE_LOG(LogTemp, Log, TEXT("LevelManager found by GameState in level already. "));

		LevelManager = Cast<ALevelManager>(LevelManagerCandidates[0]);
	}
	else
	{
		LevelManager = Cast<ALevelManager>(LevelManagerCandidates[0]);

		UE_LOG(LogTemp, Warning, TEXT("More than one LevelManagers found by GameState. This will cause undefined behaviour. Selecting first (%s)"),
			*LevelManager->GetName());
	}

	//LevelManager->GameState = this;
}

void APizzaGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasHadFirstTick == false)
		PostBeginPlay();

	float PassedTime = UpdateGameTime(DeltaTime);

	UpdateBankruptPlayers(PassedTime);
}

float APizzaGameState::UpdateGameTime(float DeltaTime)
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

	return DeltaTime * TimeSpeed;
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
	UE_LOG(LogTemp, Log, TEXT("Charging %d players for upkeep..."), Players.Num());

	for (APizzaPlayer* Player : Players)
	{
		Player->AddOrRemoveFunds(Player->Nodes.Num() * PerNodeUpkeep * -1);
		UE_LOG(LogTemp, Log, TEXT("This month's upkeep for player %s: %d (%d towers * %d$/mo)"),
			*Player->GetName(), Player->Nodes.Num() * PerNodeUpkeep, Player->Nodes.Num(), PerNodeUpkeep);
	}
}

FString APizzaGameState::GetTimestamp(bool IncludeTimeOfDay)
{
	if(IncludeTimeOfDay)
		return FString::Printf(TEXT("%07.2f:%02d/%02d/%02d/%04d"), TimeOfDay, Day, Week, Month, Year);
	else
		return FString::Printf(TEXT("%02d/%02d/%02d/%04d"), Day, Week, Month, Year);
}

void APizzaGameState::UpdatePlayerBankruptcy(APizzaPlayer* Player, bool EnteringBankruptcy)
{
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Weird error that should never happen! Error Code: Nuts!"));
		return;
	}

	if (EnteringBankruptcy) 
	{
		UE_LOG(LogTemp, Log, TEXT("Adding Player %s to BankruptPlayers"), *Player->GetName());
		BankruptPlayers.Add(Player, 0.0f);
		//BankruptPlayers[Player] = 0.0f;
		//BankruptPlayers.Emplace(Player, 0.0f);
	}
	else 
	{
		UE_LOG(LogTemp, Log, TEXT("Removing Player %s from BankruptPlayers"), *Player->GetName());
		BankruptPlayers.Remove(Player);
	}
}

void APizzaGameState::UpdateBankruptPlayers(float DeltaGameTime)
{
	for (auto& Pair : BankruptPlayers)
	{
		Pair.Value += DeltaGameTime;

		if (Pair.Value > MaxBankruptcyTime)
		{
			UE_LOG(LogTemp, Log, TEXT("Player %s has been bankrupt too long and must die!"), *Pair.Key->GetName());
			Pair.Key->OnBankruptcyMaxed();
		}
	}
}