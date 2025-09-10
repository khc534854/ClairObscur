// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTimingComponent.h"


// Sets default values for this component's properties
UBattleTimingComponent::UBattleTimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleTimingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBattleTimingComponent::StartTimingEvent(float Duration, float SuccessWindowStart, float SuccessWindowEnd)
{
	FinishTime = Duration;
	SuccessStart = SuccessWindowStart;
	SuccessEnd = SuccessWindowEnd;
	CurrentTime = 0.f;
	bIsTimingActive = true;
}

void UBattleTimingComponent::OnPlayerInput()
{
	if (!bIsTimingActive) return;

	// 타이밍 성공/실패 판정
	if (CurrentTime >= SuccessStart && CurrentTime <= SuccessEnd)
	{ 
		OnTimingResult.Broadcast(true); // 성공 방송
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Timing Success"));
	}
	else
	{
		OnTimingResult.Broadcast(false); // 실패 방송
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timing Fail"));
	}
	bIsTimingActive = false; // 한 번 판정하면 타이머 중지
}

float UBattleTimingComponent::GetCurrentTimingPercent() const
{
	if (FinishTime <= 0.f) return 0.f;
	return FMath::Clamp(CurrentTime / FinishTime, 0.f, 1.f);
}


// Called every frame
void UBattleTimingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTimingActive)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime >= FinishTime)
		{
			// 시간이 다 되면 입력이 없었으므로 실패 처리
			OnTimingResult.Broadcast(false);
			bIsTimingActive = false;
		}
	}
}

