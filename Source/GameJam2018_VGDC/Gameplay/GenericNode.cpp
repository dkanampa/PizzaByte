// Fill out your copyright notice in the Description page of Project Settings.

#include "GenericNode.h"

AGenericNode::AGenericNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AGenericNode::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGenericNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

