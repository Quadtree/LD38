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
		float forwardSpeed = pawn->GetActorRotation().GetInverse().RotateVector(Cast<UPrimitiveComponent>(pawn->GetRootComponent())->GetPhysicsLinearVelocity()).X;

		/*if (auto pc = Cast<UPrimitiveComponent>(pawn->GetRootComponent()))
		{
			pc->SetAngularDamping(2);
		}*/

		pawn->MoveForward(0);

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
			DrawDebugLine(GetWorld(), pawn->GetActorLocation() + FVector(0,0,200), NextWaypoint->GetActorLocation() + FVector(0, 0, 200), FColor::Red, false, Delta * 3, 0, 3);
			DrawDebugSphere(GetWorld(), NextWaypoint->GetActorLocation(), 100, 3, FColor::Red, false, Delta * 3, 0, 3);

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

			FVector destUnitVector = (destLocation - myLocation).GetSafeNormal();
			FVector facingUnitVector = frameRotator.RotateVector(pawn->GetActorRotation().RotateVector(FVector(0, 1, 0)));

			float angleToDest = FVector::DotProduct(destUnitVector, facingUnitVector);

			//float topSpeed = 300;

			//if (FMath::Abs(angleToDest) < 0.06f) topSpeed = 2000;

			float topSpeed = FMath::Max(300.f, 2000.f - FMath::Abs(angleToDest) * 4000.f);

			topSpeed = FMath::Min(FVector::Dist(NextWaypoint->GetActorLocation(), pawn->GetActorLocation()), topSpeed);

			if (ReverseTime <= 0)
			{
				if (forwardSpeed < topSpeed) pawn->MoveForward(1);
			}
			else
			{
				pawn->MoveForward(-1);
			}

			DrawDebugString(pawn->GetWorld(), pawn->GetActorLocation(), FString::SanitizeFloat(angleToDest), nullptr, FColor::Red, Delta, true);
			DrawDebugString(pawn->GetWorld(), pawn->GetActorLocation() + FVector(0,0,200), FString::SanitizeFloat(StuckTime), nullptr, FColor::Red, Delta, true);
			DrawDebugString(pawn->GetWorld(), pawn->GetActorLocation() + FVector(0, 0, 400), FString::SanitizeFloat(topSpeed), nullptr, FColor::Red, Delta, true);

			//UE_LOG(LogTemp, Display, TEXT("NAV %s %s %s"), *frameRotator.ToCompactString(), *myLocation.ToCompactString(), *destLocation.ToCompactString());

			float centerDist = FVector::DistSquared(centerPoint, destLocation);
			float leftDist = FVector::DistSquared(leftPoint, destLocation);
			float rightDist = FVector::DistSquared(rightPoint, destLocation);

			float rotationSpeed = 0;

			if (auto pc = Cast<UPrimitiveComponent>(pawn->GetRootComponent()))
			{
				FVector rot = pc->GetComponentRotation().GetInverse().RotateVector(pc->GetPhysicsAngularVelocity());
				rotationSpeed = rot.Z;

				//DrawDebugString(pawn->GetWorld(), pawn->GetActorLocation(), rot.ToString(), nullptr, FColor::Red, Delta, true);
			}

			pawn->MoveRight(0);

			if (angleToDest < -0.03f && rotationSpeed > -30)
			{
				pawn->MoveRight(angleToDest * 2);
			}

			if (angleToDest > 0.03f && rotationSpeed < 30)
			{
				pawn->MoveRight(angleToDest * 2);
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

		

		//DrawDebugString(pawn->GetWorld(), pawn->GetActorLocation(), FString::SanitizeFloat(forwardSpeed), nullptr, FColor::Red, Delta, true);

		ReverseTime -= Delta;

		if (forwardSpeed < 50 && ReverseTime <= 0)
		{
			StuckTime += Delta;
		}
		else
		{
			StuckTime = FMath::Max(0.f, StuckTime - Delta * 6);
		}

		if (StuckTime > 4)
		{
			if (FMath::RandRange(0, 1) != 0)
			{
				StuckTime = 0;
				ReverseTime = 8;
			}
			else
			{
				pawn->ResetToLastCheckpoint();
			}
		}
	}
}
