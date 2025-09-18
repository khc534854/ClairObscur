// Fill out your copyright notice in the Description page of Project Settings.


#include "CostWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCostWidget::SetCost(FText currentCost)
{
	Cost->SetText(currentCost);
}

void UCostWidget::SetColor(bool canUse)
{
	if (!CostBG)
	{
		return;
	}

	if (canUse)
	{
		// true일 때의 색상 (파란색 계열)
		const FLinearColor CanUseColor = FLinearColor(0.001763f, 0.076595f, 0.338542f, 1.0f);
		CostBG->SetColorAndOpacity(CanUseColor);
	}
	else
	{
		// false일 때의 색상 (붉은색 계열)
		const FLinearColor CannotUseColor = FLinearColor(0.338542f, 0.026383f, 0.025898f, 1.0f);
		CostBG->SetColorAndOpacity(CannotUseColor);
	}
}
