// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDistrict.h"
#include "LevelManager.generated.h"

class UBoxComponent;

UCLASS()
class GAMEJAM2018_VGDC_API ALevelManager : public AActor
{
	GENERATED_BODY()

private:
	bool HasHadFirstTick = false; // Used as a slightly-post-begin-play
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*UFUNCTION(BlueprintCallable)
		void SpawnNodeColliders();*/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D")
		TArray<FDistrict> Districts;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D")
		FVector ColliderScale = FVector(1.0f, 1.0f, 2.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D")
		TMap<UBoxComponent*, FBlock> BlockNodeColliders;
};
