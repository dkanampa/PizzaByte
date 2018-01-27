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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		EPizzaTopping PizzaType;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FBlock Block;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float TimePlaced;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float ExpireTime;
};