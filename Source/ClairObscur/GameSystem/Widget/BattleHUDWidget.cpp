// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "WidgetComponent/CostBarWidget.h"
#include "WidgetComponent/CostWidget.h"

void UBattleHUDWidget::UpdatePlayerHP(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.f) return;
	if (!ProgressBarPlayerHP) return;

	const float Percent = CurrentHP / MaxHP;
	ProgressBarPlayerHP->SetPercent(Percent);
}

void UBattleHUDWidget::UpdateBossHP(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.f) return;
	if (!ProgressBarBossHP) return;

	const float Percent = CurrentHP / MaxHP;
	ProgressBarBossHP->SetPercent(Percent);
}

void UBattleHUDWidget::UpdateCostBar(int32 Cost)
{
	if (!CostBar) return;
	CostBar->SetCost(Cost);
}

void UBattleHUDWidget::UpdateCostText(int32 Cost)
{
	if (!WBP_Cost) return;
	WBP_Cost->SetCost(FText::AsNumber(Cost));
}

void UBattleHUDWidget::UpdatePlayerHPText(float CurrentHP, float MaxHP)
{
	if (!TextPlayerHP) return;

	FFormatNamedArguments Args;
	Args.Add(TEXT("Current"), FMath::RoundToInt(CurrentHP)); // 소수점을 정수로 변환
	Args.Add(TEXT("Max"), FMath::RoundToInt(MaxHP));

	FText HPText = FText::Format(NSLOCTEXT("BattleHUD", "HPTextFormat", "{Current} / {Max}"), Args);

	TextPlayerHP->SetText(HPText);
}