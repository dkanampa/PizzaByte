#pragma once
#include "CoreMinimal.h"
#include "Environment/FBlock.h"
#include "EPizzaTopping.h"
#include "FOrder.generated.h"

USTRUCT(BlueprintType)
struct FOrder
{
	GENERATED_USTRUCT_BODY();
	EPizzaTopping PizzaType;
	FBlock Block;
	float TimePlaced;
	float ExpireTime;
};