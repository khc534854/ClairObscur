// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API USkillDetailWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetSkillDetails(struct FSkillRow SkillData, int32 skillNum);
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SkillName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SkillDetail;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeyText;
	UPROPERTY(meta = (BindWidget))
	class UCostWidget* CostWidget;
};
