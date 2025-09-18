// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectSkillWidget.h"

#include "CharacterComponent/SkillRow.h"
#include "WidgetComponent/SkillDetailWidget.h"

// void USelectSkillWidget::SetSkillDatas()
// {
// 	SkillDetail1->SetSkillDetails();
// 	SkillDetail2->SetSkillDetails();
// }

void USelectSkillWidget::PopulateSkills(const TArray<FSkillRow*>& SkillRows, APlayerBase* curPlayer)
{
	TArray<USkillDetailWidget*> DetailWidgets = { SkillDetail1, SkillDetail2 };

	for (int32 i = 0; i < DetailWidgets.Num(); i++)
	{
		// 전달받은 스킬 데이터가 있고, 위젯도 유효하다면
		if (DetailWidgets[i] && SkillRows.IsValidIndex(i + 1) && SkillRows[i + 1] != nullptr)
		{
			DetailWidgets[i]->SetSkillDetails(*SkillRows[i + 1], i + 1, curPlayer);
			DetailWidgets[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else if(DetailWidgets[i])
		{
			DetailWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
