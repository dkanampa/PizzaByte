#pragma once
#include "CoreMinimal.h"
#include "FSector.h"
#include "../Gameplay/EPizzaTopping.h"
#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "FDistrict.generated.h"

UENUM(BlueprintType)
enum class EDistrictType : uint8
{
	College,
	Suburb,
	Business
};

USTRUCT(BlueprintType) // UE4-specific macro, do not delete!
struct FDistrict
{
	GENERATED_USTRUCT_BODY(); // UE4-specific macro, do not delete!

public:
	
	FDistrict() {};

	FDistrict(EDistrictType _Type) :Type(_Type) {};


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName DistrictName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FSector> Sectors;

	// Each building is just an instance of some basic cube mesh with varying 
	//   scales.
	// InstancedStaticMesh allows for stupidly efficient rendering of thousands 
	//   of these, with material variations like light color able to be 
	//    randomized per - building
	// Done at a district level to allow for frustrum culling of districts not in
	//   sight, since this isn’t done per - instance; also allows all buildings of
	//   a district to be a different color(College town = brick = red, 
	//   suburb = vinyl siding = off white, etc.)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UInstancedStaticMeshComponent* BuildingMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EDistrictType Type;

	// That is, a map of pizza toppings to likelihood between 0 and 1 of the 
	//   topping being chosen when a new order is placed
	// Any pizza topping not included is assumed to have a frequency of 0 and 
	//   thus will never be ordered
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<EPizzaTopping, float> ToppingPreferences;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 PropertyRate;

private:
	// etc..

};