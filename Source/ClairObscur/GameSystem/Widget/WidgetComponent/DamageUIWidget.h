// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UDamageUIWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	// 데미지 색상 및 애니메이션 재생
	UFUNCTION(BlueprintCallable)
	void SetDamageAndColor(int32 InDamage, const FLinearColor& InColor);
	
	UFUNCTION(BlueprintCallable)
	void PlayDamageAnim(float Duration = 1.0f, bool bCritical = false);

	
	virtual void NativeConstruct() override;

	// 데미지 숫자
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Txt_Damage = nullptr;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* TextSizeControl = nullptr;

	
	UFUNCTION(BlueprintCallable)
	void SetPopupColor(FString DodgeType, const FLinearColor& InColor);
	

	
	
};
