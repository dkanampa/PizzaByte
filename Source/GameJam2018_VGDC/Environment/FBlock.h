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

	FVector GetBlockCenter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D")
		TArray<FVector> NodeLocations;

	/// For simplicity sake we'll implement this later and just give
	///   or level designer an array of vectors to implement
	// Positions in 3d where towers can go, and a boolean flag indicating
	//   whether that spot is occupied
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D")
	//	TMap<FVector, bool> TowerSpots;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName ParentSectorName;

	FSector* ParentSector;

	// Pointer means we can have it null indicating no tower present
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		APizzaNode* OccupyingNode;
};