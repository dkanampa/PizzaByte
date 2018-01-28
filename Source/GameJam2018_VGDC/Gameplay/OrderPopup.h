// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FOrder.h"

#include "GameFramework/Actor.h"
#include "OrderPopup.generated.h"

/**
 * Actor in the 3D world that pops up where new orders are spawned
 * Spawned by OrderMananger, clickable by the player
 */
UCLASS()
class GAMEJAM2018_VGDC_API AOrderPopup : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrderPopup();

protected:
	virtual void BeginPlay() override;

public:	
	USceneComponent* OriginPlaceholder;
	//virtual void Tick(float DeltaTime) override;

	// Remember to pass by reference!
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FOrder Order;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsSelected;
};
