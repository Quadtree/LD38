// Fill out your copyright notice in the Description page of Project Settings.

#include "LD38.h"
#include "OpponentCarController.h"
#include "LD38Pawn.h"
#include "EngineUtils.h"
#include "Checkpoint.h"
#include "Waypoint.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

void AOpponentCarController::Tick(float Delta)
{
	auto pawn = Cast<ALD38Pawn>(this->GetPawn());

	if (pawn)
	{
		pawn->MoveForward(1);

		if (NextWaypoint == nullptr)
		{
			float bestDistSqr = TNumericLimits<float>::Max();

			for (TActorIterator<ACheckpoint> i(GetWorld()); i; ++i)
			{
				float distSqr = FVector::DistSquared(i->GetActorLocation(), pawn->GetActorLocation());

				if (distSqr < bestDistSqr && i->CheckpointNumber == pawn->NextCheckpoint)
				{
					NextWaypoint = *i;
					bestDistSqr = distSqr;
				}
			}

			for (TActorIterator<AWaypoint> i(GetWorld()); i; ++i)
			{
				float distSqr = FVector::DistSquared(i->GetActorLocation(), pawn->GetActorLocation());

				if (distSqr < bestDistSqr && i->NextCheckpointNumber == pawn->NextCheckpoint)
				{
					NextWaypoint = *i;
					bestDistSqr = distSqr;
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
			FVector centerPoint = frameRotator.RotateVector(pawn->GetActorLocation() + pawn->GetActorRotation().RotateVector(FVector(300, 0, 0)));
			FVector leftPoint = frameRotator.RotateVector(pawn->GetActorLocation() + pawn->GetActorRotation().RotateVector(FVector(300, -100, 0)));
			FVector rightPoint = frameRotator.RotateVector(pawn->GetActorLocation() + pawn->GetActorRotation().RotateVector(FVector(300, 100, 0)));
			
			FVector destLocation = frameRotator.RotateVector(NextWaypoint->GetActorLocation());

			myLocation.Z = 0;
			destLocation.Z = 0;
			centerPoint.Z = 0;
			leftPoint.Z = 0;
			rightPoint.Z = 0;

			//UE_LOG(LogTemp, Display, TEXT("NAV %s %s %s"), *frameRotator.ToCompactString(), *myLocation.ToCompactString(), *destLocation.ToCompactString());

			float centerDist = FVector::DistSquared(centerPoint, destLocation);
			float leftDist = FVector::DistSquared(leftPoint, destLocation);
			float rightDist = FVector::DistSquared(rightPoint, destLocation);

			if (leftDist < centerDist && leftDist < rightDist)
			{
				pawn->MoveRight(-1);
			}

			if (rightDist < centerDist && rightDist < leftDist)
			{
				pawn->MoveRight(1);
			}

			if (auto chk = Cast<ACheckpoint>(NextWaypoint))
			{
				if (chk->CheckpointNumber != pawn->NextCheckpoint) NextWaypoint = nullptr;
			}

			if (auto wpt = Cast<AWaypoint>(NextWaypoint))
			{
				if (FVector::DistSquared(wpt->GetActorLocation(), pawn->GetActorLocation()) < FMath::Square(800))
				{
					int32 targetNextWaypoint = wpt->WaypointNumber + 1;
					NextWaypoint = nullptr;

					for (TActorIterator<AWaypoint> i(GetWorld()); i; ++i)
					{
						float distSqr = FVector::DistSquared(i->GetActorLocation(), pawn->GetActorLocation());

						if (i->NextCheckpointNumber == pawn->NextCheckpoint && i->WaypointNumber == targetNextWaypoint)
						{
							NextWaypoint = *i;
						}
					}

					if (!NextWaypoint)
					{
						for (TActorIterator<ACheckpoint> i(GetWorld()); i; ++i)
						{
							if (i->CheckpointNumber == pawn->NextCheckpoint)
							{
								NextWaypoint = *i;
							}
						}
					}
				}
			}
		}

		float forwardSpeed = pawn->GetActorRotation().GetInverse().RotateVector(Cast<UPrimitiveComponent>(pawn->GetRootComponent())->GetPhysicsLinearVelocity()).X;

		DrawDebugString(pawn->GetWorld(), pawn->GetActorLocation(), FString::SanitizeFloat(forwardSpeed), nullptr, FColor::Red, Delta, true);
	}
}
