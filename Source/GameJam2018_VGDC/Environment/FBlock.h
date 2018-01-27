#pragma once
#include "CoreMinimal.h"
#include "FBlock.generated.h" 

struct FSector; // Forward declare to avoid circular includes

USTRUCT(BlueprintType) 
struct FBlock
{
	GENERATED_USTRUCT_BODY(); 

public:
	FBlock() {};

	FBlock(FSector* _ParentSector) :ParentSector(_ParentSector) {};

	// Pointer means we can have it null indicating no tower present
	FPizzaNode* OccupyingNode;

	FSector* ParentSector;

};