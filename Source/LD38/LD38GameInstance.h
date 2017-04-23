// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "LD38GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LD38_API ULD38GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = CarTypes)
	TArray<TSubclassOf<class ALD38Pawn>> CarTypes;
	
	UPROPERTY(EditAnywhere, Category = CarTypes, BlueprintReadWrite)
	TSubclassOf<class ALD38Pawn> SelectedCarType;
};
