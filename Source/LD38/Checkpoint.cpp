// Fill out your copyright notice in the Description page of Project Settings.

#include "LD38.h"
#include "Checkpoint.h"
#include "LD38Pawn.h"


// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlap);
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::OnOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor) { UE_LOG(LogTemp, Display, TEXT("Overlapping %s"), *OtherActor->GetName()); }

	if (auto p = Cast<ALD38Pawn>(OtherActor))
	{
		if (this->CheckpointNumber == p->NextCheckpoint)
		{
			p->NextCheckpoint = 1 - p->NextCheckpoint;
			UE_LOG(LogTemp, Display, TEXT("%s's next checkpoint is %s"), *OtherActor->GetName(), *FString::FromInt(p->NextCheckpoint));
		}
	}
}

