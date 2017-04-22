// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "OpponentCarController.generated.h"

/**
 * 
 */
UCLASS()
class LD38_API AOpponentCarController : public AAIController
{
	GENERATED_BODY()

	virtual void Tick(float Delta);
	
	UPROPERTY()
	class AActor* NextWaypoint;
};
