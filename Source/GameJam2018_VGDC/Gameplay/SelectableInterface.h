// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SelectableInterface.generated.h"

/**
 * Probably overengineering, but I imagine we may also want to have some way of
 *   distinguishing the most recently selected element from other selected
 *   elements, especially for materials and HUD. Blender3D has something like
 *   this.
 */
UENUM(BlueprintType)
enum class ESelectionState : uint8
{
	Deselected, 
	Selected,   // Selected, but another element is more recently selected
	Active      // The most recently selected element
};

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class USelectableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEJAM2018_VGDC_API ISelectableInterface
{
	GENERATED_BODY()
public:
	
	/**
	 * Called when the player selects/deselects this actor
	 * BlueprintNativeEvent, so you can override in Blueprints. Remember, in
	 *   C++ this means the .cpp file should only have 
	 *   UpdateSelection_Implementation(...) !
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
		void UpdateSelection(ESelectionState Type = ESelectionState::Active);

	/**
	 * @return The current selection state
	 * BlueprintNativeEvent, so you can override in Blueprints. Remember, in
	 *   C++ this means the .cpp file should only have 
	 *   GetSelectionState_Implementation(...) !
	 */
	//UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	//	ESelectionState GetSelectionState();

	/**
	 * Whether this actor is Active/Selected or Deselected. 
	 * Calls GetSelectionState() internally.
	 * BlueprintNativeEvent, so you can override in Blueprints. Remember, in
	 *   C++ this means the .cpp file should only have 
	 *   IsSelected_Implementation(...) !
	 */
	//UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	//	bool IsSelected();

};
