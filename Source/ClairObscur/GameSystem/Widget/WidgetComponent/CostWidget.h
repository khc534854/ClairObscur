// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CostWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UCostWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCost(FText currentCost);
	void SetColor(bool canUse);
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Cost;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CostBG;
};
