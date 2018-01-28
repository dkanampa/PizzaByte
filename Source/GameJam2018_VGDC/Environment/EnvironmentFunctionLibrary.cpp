// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvironmentFunctionLibrary.h"





FSector UEnvironmentFunctionLibrary::GetBlockParentSector(FBlock Block)
{
	if (Block.ParentSector != nullptr)
		return *Block.ParentSector;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetBlockParentSector called but Block has no parent sector!"));
		return FSector();
	}
}

FDistrict UEnvironmentFunctionLibrary::GetSectorParentDistrict(FSector Sector)
{
	if (Sector.ParentDistrict != nullptr)
		return *Sector.ParentDistrict;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetSectorParentDistrict called but Block has no parent district!"));
		return FDistrict();
	}
}

bool UEnvironmentFunctionLibrary::SectorHasTower(FSector Sector)
{
	for (FBlock& Block : Sector.Blocks)
	{
		if (Block.OccupyingNode != nullptr)
			return true;
	}

	return false;
}