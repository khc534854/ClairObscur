// Fill out your copyright notice in the Description page of Project Settings.


#include "CostBarWidget.h"

#include "CostWidget.h"
#include "Components/Image.h"

void UCostBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	Costs.Push(Cost1);
	Costs.Push(Cost2);
	Costs.Push(Cost3);
	Costs.Push(Cost4);
	Costs.Push(Cost5);
	Costs.Push(Cost6);
	Costs.Push(Cost7);
	Costs.Push(Cost8);
	Costs.Push(Cost9);

	for (int32 i = 0; i < Costs.Num(); i++)
	{
		Costs[i]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCostBarWidget::SetCost(int32 newCost)
{
	for (int32 i = 0; i < newCost; i++)
	{
		Costs[i]->SetVisibility(ESlateVisibility::Visible);
	}
}
