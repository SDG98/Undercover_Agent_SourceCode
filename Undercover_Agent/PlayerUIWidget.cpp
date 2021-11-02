// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUIWidget.h"



UTextBlock* UPlayerUIWidget::GetCurrentHealthLabel() {

    return CurrentHealthLabel;
}

UTextBlock* UPlayerUIWidget::GetCurrentAmmoLabel() {

    return CurrentAmmoLabel;
}



UImage* UPlayerUIWidget::GetDeathScreenTint() {

    return DeathScreenTint;
}

void UPlayerUIWidget::SetCurrentHealthLabel(int health)
{
    Health = health;
    CurrentHealthLabel->SetText(FText::FromString(FString::FromInt(Health)));
}

void UPlayerUIWidget::SetCurrentAmmoLabel(int ammo, int mAmmo)
{
    Ammo = ammo;
    MaxAmmo = mAmmo;
    CurrentAmmoLabel->SetText(FText::FromString(FString::FromInt(Ammo) + " / " + FString::FromInt(MaxAmmo)));
}


