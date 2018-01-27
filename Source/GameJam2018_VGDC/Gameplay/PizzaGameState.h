// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PizzaPlayer.h"
#include "../Environment/FDistrict.h"
#include "FOrder.h"

#include "GameFramework/GameStateBase.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "PizzaGameState.generated.h"

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
 * 
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
	
protected:
	virtual void BeginPlay() override;

public:
	APizzaGameState();
	virtual void Tick(float DeltaTime) override;

	// Handles minute/day/month incrementation
	UFUNCTION(BlueprintCallable, Category = "Time")
		void UpdateGameTime(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Time")
		bool IsWeekend();

	// Assume's PeriodOfDayTimes is sorted
	UFUNCTION(BlueprintCallable, Category = "Time")
		EPeriodOfDay GetPeriodOfDay();

	// Bills players for upkeep
	UFUNCTION(BlueprintCallable, Category = "Time")
		void OnNewMonth();	

	// Assume's Seasons map is sorted
	UFUNCTION(BlueprintCallable, Category = "Time")
		ESeason GetSeason();

	// Mainly for debugging
	UFUNCTION(BlueprintCallable, Category = "Time")
		FString GetTimestamp(bool IncludeTimeOfDay = true);

	// Called every hour; may no orders or a bunch, depending on the will of RNGesus
	UFUNCTION(BlueprintCallable, Category = "Orders")
		void GenerateNewOrders();

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

	// All players in the game; automatically queried for at the start of the 
	//   game
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Environment")
		TArray<APizzaPlayer*> Players;

	// All districts in the map; should be provided 
	// REMEMBER FOR GOD'S SAKE TO PASS BY REFERENCE!!!!!!!!!!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Environment")
		TArray<FDistrict> Districts;

	// Scans for one at the beginning of the game; or will, eventually..
	// TODO: Move sun across the sky
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Environment")
		ADirectionalLight* SunLight;
};
