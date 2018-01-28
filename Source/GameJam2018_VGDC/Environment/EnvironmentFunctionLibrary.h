// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FDistrict.h"
#include "EnvironmentFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM2018_VGDC_API UEnvironmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Makes sure parent pointers point to parents
	UFUNCTION(BlueprintCallable)
		static FSector GetBlockParentSector(FBlock Block);

	// Makes sure parent pointers point to parents
	UFUNCTION(BlueprintCallable)
		static FDistrict GetSectorParentDistrict(FSector Sector);
	
	UFUNCTION(BlueprintCallable)
		static bool SectorHasTower(FSector Sector);
};
