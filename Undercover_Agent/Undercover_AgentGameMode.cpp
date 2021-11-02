// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Undercover_AgentGameMode.h"
#include "Undercover_AgentHUD.h"
#include "Undercover_AgentCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUndercover_AgentGameMode::AUndercover_AgentGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AUndercover_AgentHUD::StaticClass();
}
