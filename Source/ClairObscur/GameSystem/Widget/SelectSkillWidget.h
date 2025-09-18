// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterComponent/SkillRow.h"
#include "SelectSkillWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API USelectSkillWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//void SetSkillDatas();

	void PopulateSkills(const TArray<FSkillRow*>& SkillRows, class APlayerBase* curPlayer);
	
public:
	UPROPERTY(meta = (BindWidget))
	class USkillDetailWidget* SkillDetail1;
	
	UPROPERTY(meta = (BindWidget))
	class USkillDetailWidget* SkillDetail2;

	//UPROPERTY(meta = (BindWidget))
	//class USkillDetailWidget* SkillDetail3;
};
