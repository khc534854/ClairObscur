// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/Character.h"
#include "WidgetComponent/CostBarWidget.h"
#include "WidgetComponent/CostWidget.h"

void UBattleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WBP_LuneTurnHUD)    TurnWidgetMap.Add(FName("Lune"), WBP_LuneTurnHUD);
	if (WBP_GustaveTurnHUD) TurnWidgetMap.Add(FName("Gustave"), WBP_GustaveTurnHUD);
	if (WBP_EnemyTurnHUD)   TurnWidgetMap.Add(FName("Enemy"), WBP_EnemyTurnHUD);
}

void UBattleHUDWidget::UpdateTurnOrderUI(int32 index)
{
	if (!TurnBox) return;

	TurnBox->ClearChildren();

	if (index == 0)
	{
		TurnBox->AddChildToVerticalBox(WBP_LuneTurnHUD);
		WBP_LuneTurnHUD->Padding.Bottom = 30;
		TurnBox->AddChildToVerticalBox(WBP_GustaveTurnHUD);
		WBP_GustaveTurnHUD->Padding.Bottom = 30;
		TurnBox->AddChildToVerticalBox(WBP_EnemyTurnHUD);
		WBP_EnemyTurnHUD->Padding.Bottom = 30;
	}
	else if (index == 1)
	{
		TurnBox->AddChildToVerticalBox(WBP_GustaveTurnHUD);
		WBP_GustaveTurnHUD->Padding.Bottom = 30;
		TurnBox->AddChildToVerticalBox(WBP_EnemyTurnHUD);
		WBP_EnemyTurnHUD->Padding.Bottom = 30;
		TurnBox->AddChildToVerticalBox(WBP_LuneTurnHUD);
		WBP_LuneTurnHUD->Padding.Bottom = 30;
	}
	else if (index == 2)
	{
		TurnBox->AddChildToVerticalBox(WBP_EnemyTurnHUD);
		WBP_EnemyTurnHUD->Padding.Bottom = 30;
		TurnBox->AddChildToVerticalBox(WBP_LuneTurnHUD);
		WBP_LuneTurnHUD->Padding.Bottom = 30;
		TurnBox->AddChildToVerticalBox(WBP_GustaveTurnHUD);
		WBP_GustaveTurnHUD->Padding.Bottom = 30;
	}

}

void UBattleHUDWidget::UpdateGustaveHP(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.f) return;
	if (!GustaveProgressBarHP) return;

	const float Percent = CurrentHP / MaxHP;
	GustaveProgressBarHP->SetPercent(Percent);
}

void UBattleHUDWidget::UpdateGustaveHPText(float CurrentHP, float MaxHP)
{
	if (!GustaveTextHP) return;

	FFormatNamedArguments Args;
	Args.Add(TEXT("Current"), FMath::RoundToInt(CurrentHP)); // 소수점을 정수로 변환
	Args.Add(TEXT("Max"), FMath::RoundToInt(MaxHP));

	FText HPText = FText::Format(NSLOCTEXT("BattleHUD", "HPTextFormat", "{Current} / {Max}"), Args);

	GustaveTextHP->SetText(HPText);
}

void UBattleHUDWidget::UpdateGustaveCostBar(int32 Cost)
{
	if (!GustaveCostBar) return;
	GustaveCostBar->SetCost(Cost);
}

void UBattleHUDWidget::UpdateGustaveCostText(int32 Cost)
{
	if (!WBP_GustaveCost) return;
	WBP_GustaveCost->SetCost(FText::AsNumber(Cost));
}


void UBattleHUDWidget::UpdateLuneHP(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.f) return;
	if (!LuneProgressBarHP) return;

	const float Percent = CurrentHP / MaxHP;
	LuneProgressBarHP->SetPercent(Percent);
}

void UBattleHUDWidget::UpdateLuneHPText(float CurrentHP, float MaxHP)
{
	if (!LuneTextHP) return;

	FFormatNamedArguments Args;
	Args.Add(TEXT("Current"), FMath::RoundToInt(CurrentHP)); // 소수점을 정수로 변환
	Args.Add(TEXT("Max"), FMath::RoundToInt(MaxHP));

	FText HPText = FText::Format(NSLOCTEXT("BattleHUD", "HPTextFormat", "{Current} / {Max}"), Args);

	LuneTextHP->SetText(HPText);
}

void UBattleHUDWidget::UpdateLuneCostBar(int32 Cost)
{
	if (!LuneCostBar) return;
	LuneCostBar->SetCost(Cost);
}

void UBattleHUDWidget::UpdateLuneCostText(int32 Cost)
{
	if (!WBP_LuneCost) return;
	WBP_LuneCost->SetCost(FText::AsNumber(Cost));
}

void UBattleHUDWidget::UpdateBossHP(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.f) return;
	if (!ProgressBarBossHP) return;

	const float Percent = CurrentHP / MaxHP;
	ProgressBarBossHP->SetPercent(Percent);
}
