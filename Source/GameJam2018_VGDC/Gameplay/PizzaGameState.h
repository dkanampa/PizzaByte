// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameStateBase.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"

#include "PizzaGameState.generated.h"

// Forward declare for circular dependencies
class APizzaOrderManager;
class APizzaPlayer;
class ALevelManager;

UENUM(BlueprintType)
enum class EPeriodOfDay : uint8
{
	Twilight,	// 12:00AM - Sunrise
	Morning,	// Sunrise - Noon
	Noon,		// Noon - Afternoonish
	Afternoon,	// Afternoonish - Sunset
	Night		// Sunset - Midnight
};

UENUM(BlueprintType)
enum class ESeason : uint8
{
	Spring,
	Summer,
	Fall,
	Winter
};

/**
 * Handles the time system and spawns the OrderManager
 */
UCLASS()
class GAMEJAM2018_VGDC_API APizzaGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	// Used for debug logging
	EPeriodOfDay TickPreviousPeriodOfDay = EPeriodOfDay::Night;

	// Used for debug logging
	ESeason TickPreviousSeason = ESeason::Winter;

	void FindOrSpawnOrderManager();
	
	void FindOrSpawnLevelManager();

	bool HasHadFirstTick = false; // Used as a slightly-post-begin-play

	void PostBeginPlay();

protected:
	virtual void BeginPlay() override;

public:
	APizzaGameState();
	virtual void Tick(float DeltaTime) override;

	/// ////////////////////////////////////////////////////////////////////// ///
	/// /// Management /////////////////////////////////////////////////////// ///
	/// ////////////////////////////////////////////////////////////////////// ///

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Management")
		TSubclassOf<APizzaOrderManager> OrderManagerClass;

	// Spawned on BeginPlay
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		APizzaOrderManager* OrderManager;

	// Spawned on BeginPlay
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Management")
		ALevelManager* LevelManager;

	// All players in the game; automatically queried for at the start of the 
	//   game
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Management")
		TArray<APizzaPlayer*> Players;

	/// ////////////////////////////////////////////////////////////////////// ///
	/// /// Time ///////////////////////////////////////////////////////////// ///
	/// ////////////////////////////////////////////////////////////////////// ///

	// Handles minute/day/month incrementation given some unscaled change in time
	//   (e.g. DeltaTime in Tick)
	// @param  DeltaTime  (Unscaled) time since last update - c.f. sim "step" size
	// @return In-game   time passed this update
	UFUNCTION(BlueprintCallable, Category = "Time")
		float UpdateGameTime(float DeltaTime);

	// Applies TimeSpeed and whatever other factors we may add to a given 
	//   DeltaTime - e.g. when finding in-game time in Tick
	// This is called in UpdateGameTime
	UFUNCTION(BlueprintCallable, Category = "Time")
		float GetScaledTime(float UnscaledTime);

	// Converts a time in minutes (i.e. what we use internally) to a string in
	//   the format "7:13PM" (or "19:13" if TwentyFourHourFormat)
	// @param Time Time in minutes (i.e. 720.0 is noon, 1440.0 is midnight)
	// @param ShowSeconds Whether to display seconds as well (Format is H:MM:SS)
	// @param TwentyFourHourFormat Whether to print with AM/PM or 24-hour clock
	// @return String in the format "7:13PM" or "19:13" 
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Time")
		static FString GetTimeAsString(float Time, bool ShowSeconds = false,
			bool TwentyFourHourFormat = false);

	// Function usually only called when UpdateGameTime detects a new month
	// Currently just bills players for upkeep
	UFUNCTION(BlueprintCallable, Category = "Time|Helper Functions")
		void OnNewMonth();

	UFUNCTION(BlueprintCallable, Category = "Time")
		bool IsWeekend();

	// Assumes PeriodOfDayTimes is sorted
	UFUNCTION(BlueprintCallable, Category = "Time")
		EPeriodOfDay GetPeriodOfDay();

	// Assumes Seasons map is sorted
	UFUNCTION(BlueprintCallable, Category = "Time")
		ESeason GetSeason();

	// Mainly for debugging
	UFUNCTION(BlueprintCallable, Category = "Time")
		FString GetTimestamp(bool IncludeTimeOfDay = true);

	// In minutes; loops back to 0 at 1440.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 0.0f, ClampMax = 1440.0f))
		float TimeOfDay = 720.0f;

	// Days in the week have no names, just numbers (starting from 1!)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 1))
		uint8 Day = 1;

	// Week of this month, starting at 1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 1))
		uint8 Week = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 1))
		uint8 Month = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 1))
		int32 Year = 1;

	// How many in-game minutes pass per real-life second
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters",
		META = (ClampMin = 0.1f, UIMax = 20.0f))
		float TimeSpeed = 5.0f;

	// Number of non-weekend days per week
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters",
		META = (ClampMin = 1, UIMax = 10))
		uint8 WeekDays = 3;

	// Number of weekend days per week
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters",
		META = (UIMax = 10))
		uint8 WeekendDays = 2;

	// Number of weeks (non-weekend days + weekend days) per month
	// Billing happens every month
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters",
		META = (UIMax = 10))
		uint8 WeeksPerMonth = 2;

	// Dictates seasonal variations in frequency orders that might just be a
	//   stretch goal
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters",
		META = (UIMax = 18))
		uint8 MonthsPerYear = 8;

	// Map of a day period : first minute (inclusive) of that period
	// E.g. <Morning, 360> means Morning starts at 360 minutes,
	//   and <Noon, 720> means morning will end at 720 exactly and Noon
	//   will start then
	// Make sure it's in order!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters")
		TMap<EPeriodOfDay, float> PeriodOfDayTimes;

	// Map of season : first month (inclusive) of that season
	// E.g. {Spring:0 , Summer:4} Spring will end and summer will begin when 
	//   month 4 starts (at midnight)
	// Make sure it's in order!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time|Parameters")
		TMap<ESeason, uint8> Seasons;

	/// ////////////////////////////////////////////////////////////////////// ///
	/// /// Money //////////////////////////////////////////////////////////// ///
	/// ////////////////////////////////////////////////////////////////////// ///

	UFUNCTION(BlueprintCallable, Category = "Fund Management")
		void UpdatePlayerBankruptcy(APizzaPlayer* Player,
			bool EnteringBankruptcy = true);

	UFUNCTION(BlueprintCallable, Category = "Fund Management")
		void UpdateBankruptPlayers(float DeltaGameTime);

	// { Player : Bankruptcy Duration }
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Fund Management")
		TMap<APizzaPlayer*, float> BankruptPlayers;

	// Max Time a player can be Bankrupt
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fund Management",
		META = (ClampMin = 0.0f))
		float MaxBankruptcyTime = 5760.0f;

	// How much $/mo upkeep is per tower per player
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fund Management")
		int32 PerNodeUpkeep = 100;

	// Scans for one at the beginning of the game; or will, eventually..
	// TODO: Move sun across the sky
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Environment")
		ADirectionalLight* SunLight;


};
