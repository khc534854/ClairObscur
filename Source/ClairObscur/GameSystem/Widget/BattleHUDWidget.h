// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UBattleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 플레이어 HP 프로그레스 바를 업데이트합니다. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerHP(float CurrentHP, float MaxHP);

	/** 보스 HP 프로그레스 바를 업데이트합니다. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateBossHP(float CurrentHP, float MaxHP);

	/** Cost Bar 위젯의 보이는 코스트 개수를 설정합니다. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateCostBar(int32 Cost);

	/** Cost 텍스트 위젯의 숫자를 설정합니다. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateCostText(int32 Cost);
    
	/** 플레이어 HP 텍스트를 "현재체력 / 최대체력" 형식으로 업데이트합니다. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerHPText(float CurrentHP, float MaxHP);
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UCostBarWidget* CostBar;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* ProgressBarPlayerHP;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* ProgressBarBossHP;


	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UCostWidget* WBP_Cost;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* TextPlayerHP;
};
