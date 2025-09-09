// Fill out your copyright notice in the Description page of Project Settings.


#include "CostWidget.h"

#include "Components/TextBlock.h"

void UCostWidget::SetCost(FText currentCost)
{
	Cost->SetText(currentCost);
}
