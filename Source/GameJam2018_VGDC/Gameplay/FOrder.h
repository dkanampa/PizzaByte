#pragma once
#include "CoreMinimal.h"
#include "EPizzaTopping.h"
#include "../Environment/FBlock.h"
#include "FOrder.generated.h"

USTRUCT(BlueprintType)
struct FOrder
{
	GENERATED_USTRUCT_BODY();

	EPizzaTopping Topping;
	FBlock Block;
	float TimePlaced;
	float ExpireTime;
};