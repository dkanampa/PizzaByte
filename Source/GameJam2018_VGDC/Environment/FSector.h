#pragma once
#include "CoreMinimal.h"
#include "FBlock.h"
#include "../Gameplay/PizzaNode.h"
#include "FSector.generated.h" 

struct FDistrict; // Forward declare to avoid circular includes

USTRUCT(BlueprintType)
struct FSector
{
	GENERATED_USTRUCT_BODY(); 

public:

	FSector() {};
	
	FSector(FDistrict* _ParentDistrict) :ParentDistrict(_ParentDistrict) {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FBlock> Blocks;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool HasPizzaNode;

	FDistrict* ParentDistrict;

};