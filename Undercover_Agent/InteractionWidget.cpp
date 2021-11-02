// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "InteractionWidget.h"

UTextBlock* UInteractionWidget::GetInteractionLabel() {
    return InteractionLabel;
}



void UInteractionWidget::SetInteractionLabel(FString interaction)
{
    InteractionLabel->SetText(FText::FromString(interaction));
}