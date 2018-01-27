// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPizzaTopping.h"
#include "PizzaNode.generated.h"

UCLASS()
class GAMEJAM2018_VGDC_API APizzaNode : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EPizzaTopping Topping;

public:
	// Sets default values for this actor's properties
	APizzaNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};