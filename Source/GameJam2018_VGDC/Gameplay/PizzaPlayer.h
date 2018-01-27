// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PizzaPlayer.generated.h"
#include "FPizzaNode.h"
#include "Environment/FDistrict.h"
#include "Environment/FSector.h"
#include "FOrder.h"

UCLASS()
class GAMEJAM2018_VGDC_API APizzaPlayer : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	int32 Funds;
	TArray<FPizzaNode> Nodes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	TArray<FDistrict*> PermittedDistricts;
protected:
	virtual void BeginPlay() override;

public:	
	APizzaPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool PurchaseTowerInSector(FSector* Sector);
	bool PursueOrder(FOrder& Order);
	bool hasBoughtSectorPermit(FDistrict* District)
};
