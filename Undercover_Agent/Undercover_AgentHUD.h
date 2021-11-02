// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.h"
#include "InteractionWidget.h"
#include "ObjectiveWidget.h"
#include "Undercover_AgentHUD.generated.h"

UCLASS()
class AUndercover_AgentHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUndercover_AgentHUD();

	/** Primary draw call for the HUD */
		/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UUserWidget> HUDWidgetClass;



	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UUserWidget> ObjectiveWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UUserWidget> InteractionWidgetClass;


	UPROPERTY(EditAnywhere, Category = "UI")
		class UPlayerUIWidget* CurrentWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
		class UObjectiveWidget* ObjectiveWidget;


	UPROPERTY(EditAnywhere, Category = "UI")
		class UInteractionWidget* InteractionWidget;


	UTextBlock* GetPlayerHealth();
	UTextBlock* GetPlayerAmmo();
	UTextBlock* GetCurrentObjective();
	UTextBlock* GetInteractionLabel();

	UImage* GetDeathScreenTint();
	void SetPlayerHealth(int health);
	void SetCurrentObjective(FString objective);

	void SetInteractionLabel(FString interaction);
	void SetPlayerAmmo(int Ammo, int mAmmo);
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

