// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleEndWidget.h"
#include "Components/TextBlock.h"
#include "GameSystem/Component/BattleResultDataComponent.h"


void UBattleEndWidget::ApplyResult(const FBattleResult& R)
{
	if (HighestDamage)
		HighestDamage->SetText(FText::AsNumber(R.MaxHitDamage));
	if (DamageDealt)
		DamageDealt->SetText(FText::AsNumber(R.TotalDamage));
	if (Time)
		Time->SetText(FText::FromString(FString::Printf(TEXT("%.1f s"), R.BattleSeconds)));
	if (SuccessfulParries)
		SuccessfulParries->SetText(FText::AsNumber(R.SuccessParryCount));
	if (SuccessfulDodges)
		SuccessfulDodges->SetText(FText::AsNumber(R.SuccessDodgeCount));
}
