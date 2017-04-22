// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "LD38.h"
#include "LD38GameMode.h"
#include "LD38Pawn.h"
#include "LD38Hud.h"

ALD38GameMode::ALD38GameMode()
{
	DefaultPawnClass = ALD38Pawn::StaticClass();
	HUDClass = ALD38Hud::StaticClass();
}
