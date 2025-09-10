// Fill out your copyright notice in the Description page of Project Settings.


#include "QTEWidget.h"

#include "FindInBlueprints.h"
#include "Components/ProgressBar.h"
#include "GameSystem/Component/BattleTimingComponent.h"

void UQTEWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OwnerTimingComponent) // OwnerTimingComponent는 외부에서 설정해줘야 함
	{
		if (OwnerTimingComponent->bIsTimingActive)
			TimingProgressBar->SetPercent(OwnerTimingComponent->GetCurrentTimingPercent());
		else
		{
			
		}
	}
}

void UQTEWidget::SetOwnerTimingComponent(UBattleTimingComponent* InTimingComponent)
{
	OwnerTimingComponent = InTimingComponent;
}

