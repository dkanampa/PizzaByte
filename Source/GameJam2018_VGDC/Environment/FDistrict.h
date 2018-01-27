#pragma once
#include "CoreMinimal.h"
#include "FDistrict.generated.h" // UE4-specific header, do not delete, even if it says it doesn't exist!

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

	FDistrict(); // Default empty constructor not really necessary

private:
	// etc..

};