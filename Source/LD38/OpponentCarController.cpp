// Fill out your copyright notice in the Description page of Project Settings.

#include "LD38.h"
#include "OpponentCarController.h"
#include "LD38Pawn.h"

void AOpponentCarController::Tick(float Delta)
{
	auto pawn = Cast<ALD38Pawn>(this->GetPawn());

	if (pawn)
	{
		pawn->MoveForward(1);
	}
}
