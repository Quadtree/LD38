// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "LD38.h"
#include "LD38GameMode.h"
#include "LD38Pawn.h"
#include "LD38Hud.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "LD38GameInstance.h"
#include "Kismet/GameplayStatics.h"



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
		for (int i = 0; i <= 3; ++i)
		{
			int32 j = i;

			if (i == 3) j = -1;

			FVector startPos = start->GetActorLocation() + start->GetActorRotation().RotateVector(FVector(0, 400, 0) * j);

			if (auto inst = Cast<ULD38GameInstance>(GetWorld()->GetGameInstance()))
			{
				TSubclassOf<ALD38Pawn> typeToSpawn = inst->CarTypes[FMath::RandRange(0, inst->CarTypes.Num() - 1)];

				if (i == 0)
				{
					if (auto gi = Cast<ULD38GameInstance>(GetWorld()->GetGameInstance()))
					{
						if (gi->SelectedCarType)
						{
							typeToSpawn = gi->SelectedCarType;
						}
					}
				}

				auto atr = GetWorld()->SpawnActor<ALD38Pawn>(typeToSpawn, startPos, start->GetActorRotation());

				if (atr)
				{
					if (i != 0)
					{
						atr->SpawnDefaultController();
					}
					else
					{
						UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(atr);
					}
				}
			}
		}
	}

	/*if (auto gi = Cast<ULD38GameInstance>(GetWorld()->GetGameInstance()))
	{
		if (gi->SelectedCarType)
		{
			DefaultPawnClass = gi->SelectedCarType;
		}
	}*/
}
