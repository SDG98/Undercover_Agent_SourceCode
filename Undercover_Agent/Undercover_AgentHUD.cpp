// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Undercover_AgentHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Engine/GameEngine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AUndercover_AgentHUD::AUndercover_AgentHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	static ConstructorHelpers::FClassFinder<UPlayerUIWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/Player_UI"));

	static ConstructorHelpers::FClassFinder<UObjectiveWidget> ObjectiveBar(TEXT("/Game/FirstPerson/UI/Objective_UI"));
	
	static ConstructorHelpers::FClassFinder<UInteractionWidget> InteractionBar(TEXT("/Game/FirstPerson/UI/Interaction_UI"));

	HUDWidgetClass = HealthBarObj.Class;
	ObjectiveWidgetClass = ObjectiveBar.Class;
	InteractionWidgetClass = InteractionBar.Class;
}


void AUndercover_AgentHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.50f, Canvas->ClipY * 0.50f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

}


void AUndercover_AgentHUD::BeginPlay() {
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UPlayerUIWidget>(GetWorld(), HUDWidgetClass);
		CurrentWidget->GetDeathScreenTint()->SetVisibility(ESlateVisibility::Hidden);

		ObjectiveWidget = CreateWidget<UObjectiveWidget>(GetWorld(), ObjectiveWidgetClass);
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);

	}

	if (CurrentWidget)
	{
		ObjectiveWidget->AddToViewport();
		InteractionWidget->AddToViewport();

		CurrentWidget->AddToViewport();
	
	}



}
void AUndercover_AgentHUD::SetPlayerHealth(int health)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("value is: %d"), health));
	if (CurrentWidget)
	{
		CurrentWidget->SetCurrentHealthLabel(health);
	}

}






void AUndercover_AgentHUD::SetPlayerAmmo(int Ammo, int mAmmo)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("value is: %d"), Ammo));

	if (CurrentWidget)
	{
		CurrentWidget->SetCurrentAmmoLabel(Ammo, mAmmo);
	}
}



void AUndercover_AgentHUD::SetCurrentObjective(FString text)
{

	if (ObjectiveWidget)
	{
		ObjectiveWidget->SetCurrentObjectiveLabel(text);
	}
}


void AUndercover_AgentHUD::SetInteractionLabel(FString interaction)
{

	if (CurrentWidget)
	{
		InteractionWidget->SetInteractionLabel(interaction);
	}

}
UTextBlock* AUndercover_AgentHUD::GetPlayerAmmo()
{

	return CurrentWidget->GetCurrentAmmoLabel();

}


UTextBlock* AUndercover_AgentHUD::GetPlayerHealth()
{

	return CurrentWidget->GetCurrentHealthLabel();

}

UTextBlock* AUndercover_AgentHUD::GetInteractionLabel()
{

	return InteractionWidget->GetInteractionLabel();

}

UTextBlock* AUndercover_AgentHUD::GetCurrentObjective()
{

	return ObjectiveWidget->GetCurrentObjectiveLabel();

}


UImage* AUndercover_AgentHUD::GetDeathScreenTint() {
	return CurrentWidget->GetDeathScreenTint();
}
