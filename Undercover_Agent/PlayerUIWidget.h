// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PlayerUIWidget.generated.h"


/**
 *
 */
UCLASS(Abstract)
class UNDERCOVER_AGENT_API UPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UImage* GetDeathScreenTint();
	UTextBlock* GetCurrentHealthLabel();
	UTextBlock* GetCurrentAmmoLabel();
	void SetCurrentHealthLabel(int health);
	void SetCurrentAmmoLabel(int ammo, int mAmmo);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int MaxAmmo;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentHealthLabel;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentAmmoLabel;

	UPROPERTY(meta = (BindWidget))
		class UImage* DeathScreenTint;

};
