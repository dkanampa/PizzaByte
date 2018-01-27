// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Environment/FDistrict.h"
#include "FOrder.h"
#include "GameFramework/GameStateBase.h"
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

/**
 * 
 */
UCLASS()
class GAMEJAM2018_VGDC_API APizzaGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	// Used for debug logging
	EPeriodOfDay TickPreviousPeriodOfDay = EPeriodOfDay::Twilight;
	
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

	// How many in-game minutes pass per real-life second
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 0.1f, UIMax = 20.0f))
		float TimeSpeed = 2.0f;

	// Number of non-weekend days per week
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 1, UIMax = 10))
		uint8 WeekDays = 3;

	// Number of weekend days per week
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (UIMax = 10))
		uint8 WeekendDays = 2;

	// Number of weeks (non-weekend days + weekend days) per month
	// Billing happens every month
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (UIMax = 10))
		uint8 WeeksPerMonth = 2;

	// Map of a day period : first minute (inclusive) of that period
	// E.g. <Morning, 360> means Morning starts at 360 minutes,
	//   and <Noon, 720> means morning will end at 720 exactly and Noon
	//   will start then
	// Make sure it's in order!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
		TMap<EPeriodOfDay, float> PeriodOfDayTimes;

	// Dictates seasonal variations in frequency orders that might just be a
	//   stretch goal
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (UIMax = 52))
		uint8 MonthsPerYear = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Orders")
		TArray<FOrder> OpenOrders;

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

};
