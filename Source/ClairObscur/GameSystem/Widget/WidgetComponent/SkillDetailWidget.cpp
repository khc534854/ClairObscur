// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDetailWidget.h"

#include "CostWidget.h"
#include "CharacterComponent/SkillRow.h"
#include "Components/TextBlock.h"

void USkillDetailWidget::SetSkillDetails(FSkillRow SkillData, int32 skillNum)
{
	SkillName->SetText(SkillData.SkillName);
	SkillDetail->SetText(SkillData.SkillDescription);
	
	if (skillNum == 0)
	{
		KeyText->SetText(FText::FromString("Q"));
	}
	else if (skillNum == 1)
	{
		KeyText->SetText(FText::FromString("Q"));
	}
	else if (skillNum == 2)
	{
		KeyText->SetText(FText::FromString("W"));
	}
	else if (skillNum == 3)
	{
		KeyText->SetText(FText::FromString("E"));
	}
	CostWidget->Cost->SetText(FText::AsNumber(SkillData.AP));
}
