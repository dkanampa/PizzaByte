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

protected:
	virtual void BeginPlay() override;

public:
	APizzaPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool PurchaseTowerInSector(FSector* Sector, FBlock* ToPurchase, EPizzaTopping Topping);
	bool PurchaseDistrictPermit(FDistrict* District);
	bool PursueOrder(FOrder& Order, TArray<APizzaNode*> PizzaNodes);
	bool hasBoughtSectorPermit(FDistrict* District);
	bool PurchaseTowerWithFunds(FBlock* ToPurchase);
	int32 GetOwnedNodesSizeInDistrict(FDistrict * District);
	bool IsValidPath(EPizzaTopping Topping, TArray<APizzaNode*> Path);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Funds;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<APizzaNode*> Nodes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDistrict> PermittedDistricts;
};