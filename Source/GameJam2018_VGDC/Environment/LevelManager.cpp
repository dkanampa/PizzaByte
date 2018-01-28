// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelManager.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values
ALevelManager::ALevelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	/*if (!HasHadFirstTick)
	{
		HasHadFirstTick = true;
		SpawnNodeColliders();
	}*/

}


/*void ALevelManager::SpawnNodeColliders()
{
	for (FDistrict& District : Districts)
	{
		for (FSector& Sector : District.Sectors)
		{
			for (FBlock& Block : Sector.Blocks)
			{
				for (FVector& NodeLoc : Block.NodeLocations)
				{
					UBoxComponent* Collider = GetWorld()->SpawnActor<UBoxComponent>(FVector(NodeLoc), FRotator());
					Collider->SetWorldScale3D(ColliderScale);
					BlockNodeColliders.Add(Collider, Block);
				}
			}
		}
	}
}*/