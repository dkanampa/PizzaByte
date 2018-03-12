// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericNode.generated.h"

/**
 * We can't use the class name "ANode" because Epic uses it for one of their
 *   animation classes.
 * That is fucking stupid and frustrating. It should be AnimationNode or something;
 *   Epic shouldn't just take a common name like Node from the users. The onus
 *   should be on the user alone whether they should or shouldn't use just ANode.
 */
UCLASS()
class GAMEJAM2018_VGDC_API AGenericNode : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AGenericNode();
	virtual void Tick(float DeltaTime) override;
};
