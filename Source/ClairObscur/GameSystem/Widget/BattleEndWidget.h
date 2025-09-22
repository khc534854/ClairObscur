// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleEndWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UBattleEndWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	
	UFUNCTION(BlueprintCallable) void ApplyResult(const FBattleResult& Result);
	// 결과 업데이트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HighestDamage;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageDealt;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageReceived;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Time;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SuccessfulParries;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SuccessfulDodges;

};
 