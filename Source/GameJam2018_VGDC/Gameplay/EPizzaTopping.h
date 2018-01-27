#pragma once
#include "CoreMinimal.h"
#include "EPizzaTopping.generated.h" 

UENUM(BlueprintType)
enum class EPizzaTopping : uint8
{
	Cheese,
	Pepperoni,
	Sausage,
	Pineapple
};