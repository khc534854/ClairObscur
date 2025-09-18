// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QTEWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UQTEWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetOwnerTimingComponent(class UBattleTimingComponent* InTimingComponent);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* TimingProgressBar;

public:
	UPROPERTY()
	class UBattleTimingComponent* OwnerTimingComponent;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Timing;
	
};
