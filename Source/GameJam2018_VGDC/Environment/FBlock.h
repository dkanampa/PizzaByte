#pragma once
#include "CoreMinimal.h"
#include "../Gameplay/PizzaNode.h"
#include "FBlock.generated.h" 

struct FSector;

USTRUCT(BlueprintType) 
struct FBlock
{
	GENERATED_USTRUCT_BODY(); 

public:
	FBlock() {};

	FBlock(FSector* _ParentSector) :ParentSector(_ParentSector) {};

	FSector* ParentSector;

	// Pointer means we can have it null indicating no tower present
	APizzaNode* OccupyingNode;
};