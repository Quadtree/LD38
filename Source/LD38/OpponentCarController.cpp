// Fill out your copyright notice in the Description page of Project Settings.

#include "LD38.h"
#include "OpponentCarController.h"
#include "LD38Pawn.h"
#include "EngineUtils.h"
#include "Checkpoint.h"

void AOpponentCarController::Tick(float Delta)
{
	auto pawn = Cast<ALD38Pawn>(this->GetPawn());

	if (pawn)
	{
		pawn->MoveForward(1);

		if (NextWaypoint == nullptr)
		{
			for (TActorIterator<ACheckpoint> i(GetWorld()); i; ++i)
			{
				if (i->CheckpointNumber == pawn->NextCheckpoint)
				{
					NextWaypoint = *i;
					break;
				}
			}

			if (NextWaypoint)
			{
				UE_LOG(LogTemp, Display, TEXT("Next waypoint is %s"), *NextWaypoint->GetName());
			}
		}
		
		if (NextWaypoint)
		{
			FRotator frameRotator = (pawn->GetActorLocation().Rotation() + FRotator(90, 0, 0)).GetInverse();

			FVector myLocation = frameRotator.RotateVector(pawn->GetActorLocation());
			
			FVector destLocation = frameRotator.RotateVector(NextWaypoint->GetActorLocation());

			myLocation.Z = 0;
			destLocation.Z = 0;

			UE_LOG(LogTemp, Display, TEXT("NAV %s %s %s"), *frameRotator.ToCompactString(), *myLocation.ToCompactString(), *destLocation.ToCompactString());

			
		}
	}
}
