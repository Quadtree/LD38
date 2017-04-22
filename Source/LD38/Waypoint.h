// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Waypoint.generated.h"

UCLASS()
class LD38_API AWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaypoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Waypoint)
	class USceneComponent* RootSceneComponent;
	
	UPROPERTY(EditAnywhere, Category = Waypoint)
	int32 NextCheckpointNumber;

	UPROPERTY(EditAnywhere, Category = Waypoint)
	int32 WaypointNumber;
};
