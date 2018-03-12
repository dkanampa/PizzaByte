// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectableInterface.h"

void ISelectableInterface::UpdateSelection_Implementation(ESelectionState Type)
{
	UE_LOG(LogTemp, Error, TEXT("ISelectableInterface::UpdateSelection() not implemented/calls super!"));
}

//ESelectionState ISelectableInterface::GetSelectionState_Implementation()
//{
//	UE_LOG(LogTemp, Error, TEXT("ISelectableInterface::GetSelectionState() not implemented/calls super!"));
//	return ESelectionState::Deselected;
//}
//
//bool ISelectableInterface::IsSelected_Implementation()
//{
//	UE_LOG(LogTemp, Error, TEXT("ISelectableInterface::IsSelected() not implemented/calls super!"));
//	return false;
//}