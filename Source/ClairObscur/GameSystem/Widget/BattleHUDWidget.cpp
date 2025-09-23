// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUDWidget.h"

#include "Components/Image.h"
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

void UBattleHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ProgressBarBossDelayHP) return;

	// 현재 딜레이 바의 퍼센트를 가져옵니다.
	float CurrentDelayPercent = ProgressBarBossDelayHP->GetPercent();

	// 목표치와 현재 딜레이 바의 퍼센트가 거의 같다면 아무것도 하지 않음
	if (FMath::IsNearlyEqual(CurrentDelayPercent, DelayHP_TargetPercent))
	{
		return;
	}
    
	// 목표치보다 현재 딜레이 바가 더 높다면 (즉, 데미지를 입었다면)
	if (CurrentDelayPercent > DelayHP_TargetPercent)
	{
		// 1. 설정된 딜레이 시간(DelayHP_InitialDelay)만큼 기다립니다.
		if (DelayHP_CurrentDelay < DelayHP_InitialDelay)
		{
			DelayHP_CurrentDelay += InDeltaTime;
			return; // 아직 기다리는 중이므로 아래 로직을 실행하지 않음
		}

		// 2. 딜레이가 끝나면, 목표치를 향해 부드럽게 감소시킵니다.
		const float NewDelayPercent = FMath::FInterpTo(
			CurrentDelayPercent,      // 현재 값
			DelayHP_TargetPercent,    // 목표 값
			InDeltaTime,              // 델타 타임
			DelayHP_InterpSpeed       // 보간 속도
		);

		ProgressBarBossDelayHP->SetPercent(NewDelayPercent);
	}
	else // 목표치보다 현재 딜레이 바가 더 낮다면 (즉, 회복했다면)
	{
		// 회복하는 경우에는 딜레이 없이 즉시 따라가도록 설정
		ProgressBarBossDelayHP->SetPercent(DelayHP_TargetPercent);
	}
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

void UBattleHUDWidget::SetGustaveDeathMask(bool bDeath)
{
	if (GustaveDeathMask)
	{
		if (bDeath)
			GustaveDeathMask->SetVisibility(ESlateVisibility::Visible);
		else
			GustaveDeathMask->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBattleHUDWidget::SetLuneDeathMask(bool bDeath)
{
	if (LuneDeathMask)
	{
		if (bDeath)
			LuneDeathMask->SetVisibility(ESlateVisibility::Visible);
		else
			LuneDeathMask->SetVisibility(ESlateVisibility::Hidden);
	}
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

	if (ProgressBarBossHP)
	{
		ProgressBarBossHP->SetPercent(CurrentHP / MaxHP);
	}

	DelayHP_TargetPercent = CurrentHP / MaxHP;

	DelayHP_CurrentDelay = 0.0f;
}
