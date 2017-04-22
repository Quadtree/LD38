// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "LD38Hud.generated.h"


UCLASS(config = Game)
class ALD38Hud : public AHUD
{
	GENERATED_BODY()

public:
	ALD38Hud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
