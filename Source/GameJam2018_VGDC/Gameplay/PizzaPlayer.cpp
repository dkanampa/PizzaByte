// Fill out your copyright notice in the Description page of Project Settings.

#include "PizzaPlayer.h"


// Sets default values
APizzaPlayer::APizzaPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APizzaPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APizzaPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APizzaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

