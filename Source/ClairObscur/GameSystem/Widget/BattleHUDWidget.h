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

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void UpdateTurnOrderUI(int32 index);
	
	//Gustave
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateGustaveHP(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateGustaveHPText(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateGustaveCostBar(int32 Cost);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateGustaveCostText(int32 Cost);


	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UCostBarWidget* GustaveCostBar;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* GustaveProgressBarHP;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UCostWidget* WBP_GustaveCost;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* GustaveTextHP;

	//Lune
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateLuneHP(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateLuneHPText(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateLuneCostBar(int32 Cost);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateLuneCostText(int32 Cost);


	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UCostBarWidget* LuneCostBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* LuneProgressBarHP;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UCostWidget* WBP_LuneCost;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* LuneTextHP;

	//Boss
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateBossHP(float CurrentHP, float MaxHP);

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* ProgressBarBossHP;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* ProgressBarBossDelayHP;

private:
	float DelayHP_TargetPercent = 1.f;

	UPROPERTY(EditAnywhere, Category = "HUD|Boss HP")
	float DelayHP_InitialDelay = 0.5f;

	float DelayHP_CurrentDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "HUD|Boss HP")
	float DelayHP_InterpSpeed = 2.0f;

public:
	//turn
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UVerticalBox* TurnBox;

	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUserWidget* WBP_LuneTurnHUD;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUserWidget* WBP_GustaveTurnHUD;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUserWidget* WBP_EnemyTurnHUD;
	
	TMap<FName, UUserWidget*> TurnWidgetMap;
};
