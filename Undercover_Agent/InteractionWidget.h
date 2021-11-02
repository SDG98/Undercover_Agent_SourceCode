// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UNDERCOVER_AGENT_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()



public:

	void SetInteractionLabel(FString interaction);
	UTextBlock* GetInteractionLabel();


protected:


	UPROPERTY(meta = (BindWidget))
		class UTextBlock* InteractionLabel;
	
};
