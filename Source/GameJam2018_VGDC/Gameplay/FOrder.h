#pragma once
#include "CoreMinimal.h"
#include "FOrder.generated.h"
#include "Environment/FBlock.h"

USTRUCT(BlueprintType)
struct FOrder
{
	GENERATED_USTRUCT_BODY();
	EPizzaTopping PizzaType;
	FBlock Block;
	float TimePlaced;
	float ExpireTime;
};