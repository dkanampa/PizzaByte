#pragma once
#include "CoreMinimal.h"
#include "Environment/FBlock.h"
#include "EPizzaTopping.h"
#include "FOrder.generated.h"

USTRUCT(BlueprintType)
struct FOrder
{
	GENERATED_USTRUCT_BODY();

	FOrder() {};

	FOrder(EPizzaTopping _PizzaType, FBlock& _Block, float _TimePlaced, float _ExpireTime)
		:PizzaType(_PizzaType), Block(_Block), TimePlaced(_TimePlaced), ExpireTime(_ExpireTime) {};

	FOrder(EPizzaTopping _PizzaType, int32 _OrderCost, FBlock& _Block, float _TimePlaced, float _ExpireTime)
		:PizzaType(_PizzaType), OrderCost(_OrderCost), Block(_Block), TimePlaced(_TimePlaced), ExpireTime(_ExpireTime) {};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		EPizzaTopping PizzaType;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		int32 OrderCost = 10;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FBlock Block;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FVector ExactLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float TimePlaced;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float ExpireTime;

	// DO NOT USE! If you need a code, call OrderManager->GenerateCode()!
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString PizzaCode = "DEPRECATED";
};