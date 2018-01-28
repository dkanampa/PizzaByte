#include "FBlock.h"
#include "FSector.h"
#include "GameJam2018_VGDC.h"

FVector FBlock::GetBlockCenter()
{
	// Center of multiple points = center of bounding box
	FVector Max = FVector(0.0f);
	FVector Min = FVector(0.0f);

	for (FVector Vec : NodeLocations)
	{
		Max = FVector(
			FMath::Max(Vec.X, Max.X),
			FMath::Max(Vec.Y, Max.Y),
			FMath::Max(Vec.Z, Max.Z));

		Min = FVector(
			FMath::Min(Vec.X, Min.X),
			FMath::Min(Vec.Y, Min.Y),
			FMath::Min(Vec.Z, Min.Z));
	}

	return FVector(
		Min.X + Max.X / 2,
		Min.Y + Max.Y / 2,
		Min.Z + Max.Z / 2);
}