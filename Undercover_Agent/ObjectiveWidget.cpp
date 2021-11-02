// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ObjectiveWidget.h"




UTextBlock* UObjectiveWidget::GetCurrentObjectiveLabel() {
    return ObjectiveLabel;
}


void UObjectiveWidget::SetCurrentObjectiveLabel(FString objective)
{
    ObjectiveLabel->SetText(FText::FromString(objective));
}