// Fill out your copyright notice in the Description page of Project Settings.


#include "CostBarWidget.h"

#include "CostWidget.h"
#include "Components/Image.h"

void UCostBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeCostsArray();

	for (int32 i = 0; i < Costs.Num(); i++)
	{
		Costs[i]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCostBarWidget::SetCost(int32 newCost)
{
	InitializeCostsArray();

	for (int32 i = 0; i < newCost; i++)
	{
		Costs[i]->SetVisibility(ESlateVisibility::Visible);
	}
	for (int32 i = newCost; i < Costs.Num(); i++)
	{
		Costs[i]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCostBarWidget::InitializeCostsArray()
{
	if (bIsCostsInitialized) return;

	Costs.Empty(); // 만약을 위해 비우고 시작
	Costs.Push(Cost1);
	Costs.Push(Cost2);
	Costs.Push(Cost3);
	Costs.Push(Cost4);
	Costs.Push(Cost5);
	Costs.Push(Cost6);
	Costs.Push(Cost7);
	Costs.Push(Cost8);
	Costs.Push(Cost9);

	// null 포인터가 들어간 경우를 대비해 제거합니다.
	Costs.Remove(nullptr);

	bIsCostsInitialized = true;
}
