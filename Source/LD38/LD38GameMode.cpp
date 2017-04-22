// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "LD38.h"
#include "LD38GameMode.h"
#include "LD38Pawn.h"
#include "LD38Hud.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "LD38GameInstance.h"


ALD38GameMode::ALD38GameMode()
{
	DefaultPawnClass = ALD38Pawn::StaticClass();
	HUDClass = ALD38Hud::StaticClass();
}

void ALD38GameMode::BeginPlay()
{
	AActor *start = nullptr;

	for (TActorIterator<APlayerStart> itr(GetWorld()); itr; ++itr)
	{
		start = *itr;
	}

	if (start)
	{
		for (int i = -1; i <= 0; ++i)
		{
			if (i == 0) continue;

			FVector startPos = start->GetActorLocation() + start->GetActorRotation().RotateVector(FVector(0, 400, 0) * i);

			if (auto inst = Cast<ULD38GameInstance>(GetWorld()->GetGameInstance()))
			{
				auto atr = GetWorld()->SpawnActor<ALD38Pawn>(inst->CarTypes[FMath::RandRange(0, inst->CarTypes.Num() - 1)], startPos, start->GetActorRotation());

				if (atr)
				{
					atr->SpawnDefaultController();
				}
			}
		}
	}
}
