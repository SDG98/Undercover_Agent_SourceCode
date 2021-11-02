// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ObjectiveWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UNDERCOVER_AGENT_API UObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTextBlock* GetCurrentObjectiveLabel();
	void SetCurrentObjectiveLabel(FString objective);
protected:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ObjectiveLabel;


};
