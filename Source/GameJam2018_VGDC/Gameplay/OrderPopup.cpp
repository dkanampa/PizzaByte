// Fill out your copyright notice in the Description page of Project Settings.

#include "OrderPopup.h"


// Sets default values
AOrderPopup::AOrderPopup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OriginPlaceholder = CreateDefaultSubobject<USceneComponent>(TEXT("Placeholder Origin"));
	RootComponent = OriginPlaceholder;
}

// Called when the game starts or when spawned
void AOrderPopup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
/*void AOrderPopup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

