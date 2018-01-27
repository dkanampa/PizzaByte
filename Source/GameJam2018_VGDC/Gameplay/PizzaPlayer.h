// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Environment/FDistrict.h"
#include "Environment/FSector.h"
#include "PizzaNode.h"
#include "FOrder.h"
#include "../Environment/FSector.h"
#include "../Environment/FBlock.h"
#include "PizzaPlayer.generated.h"

UCLASS()
class GAMEJAM2018_VGDC_API APizzaPlayer : public APawn
{
	GENERATED_BODY()

private:
	int32 Funds;
	TArray<APizzaNode> Nodes;
	TArray<FDistrict*> PermittedDistricts;
protected:
	virtual void BeginPlay() override;

public:
	APizzaPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool PurchaseTowerInSector(FSector* Sector, FBlock* ToPurchase, EPizzaTopping Topping);
	bool PurchaseDistrictPermit(FDistrict* District);
	bool PursueOrder(FOrder& Order, TArray<APizzaNode> PizzaNodes);
	bool hasBoughtSectorPermit(FDistrict* District);
};
