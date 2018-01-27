// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Environment/FDistrict.h"
#include "FOrder.h"
#include "GameFramework/GameStateBase.h"
#include "PizzaGameState.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM2018_VGDC_API APizzaGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	
protected:
	virtual void BeginPlay() override;

public:
	APizzaGameState();
	virtual void Tick(float DeltaTime) override;
	
	// In minutes; loops back to 0 at 1440.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 0.0f, ClampMax = 1440.0f))
		float TimeOfDay = 720.0f;

	// How many in-game minutes pass per real-life second
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (ClampMin = 0.1f, ClampMax = 20.0f))
		float TimeSpeed = 2.0f;

	// Days in the week have no names, just numbers
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
		uint8 DayOfWeek = 0;

	// Number of non-weekend days per week
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time",
		META = (UIMax = 10))
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
