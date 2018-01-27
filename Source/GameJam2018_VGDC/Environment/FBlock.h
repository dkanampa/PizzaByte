#pragma once
#include "CoreMinimal.h"
#include "../Gameplay/PizzaNode.h"
#include "FBlock.generated.h" 

struct FSector; // Forward declare to avoid circular includes

USTRUCT(BlueprintType) 
struct FBlock
{
	GENERATED_USTRUCT_BODY(); 

public:
	FBlock() {};

	FBlock(FSector* _ParentSector) :ParentSector(_ParentSector) {};

	// I.e. map of whether the building is occupied (by another business) to
	//   where a new tower mesh can be placed
	// Stored as such because we cannot access individual mesh instances, nor 
	//   do we really need to; we just need to know where, if anywhere, we can 
	//   place a new pizza transmission tower.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<FVector, bool> Buildings;

	FSector* ParentSector;

	// Pointer means we can have it null indicating no tower present
	APizzaNode* OccupyingNode;
};