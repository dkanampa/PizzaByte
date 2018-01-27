#pragma once
#include "CoreMinimal.h"
#include "FBlock.h"
#include "../Gameplay/FPizzaNode.h"
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
	
	bool hasPizzaNode;

	FDistrict* ParentDistrict;

};