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
	bIsTimingActive = true;
	FinishTime = Duration;
	SuccessStart = SuccessWindowStart;
	SuccessEnd = SuccessWindowEnd;
	CurrentTime = 0.f;
	CurrentMode = ETimingMode::PlayerAttack;
}

void UBattleTimingComponent::StartParryTiming()
{
	bIsTimingActive = true;
	CurrentMode = ETimingMode::EnemyParry;
	bCanParry = true;
}

void UBattleTimingComponent::EndParryTiming()
{
	if(CurrentMode == ETimingMode::EnemyParry)
	{
		OnTimingResult.Broadcast(false); // 창이 닫혔을 때 누르면 실패
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parry Fail"));
		bIsTimingActive = false;
	}
	CurrentMode = ETimingMode::Inactive;
	bCanParry = false;
}

void UBattleTimingComponent::OnPlayerInput()
{
	if (!bIsTimingActive) return;

	switch (CurrentMode)
	{
	case ETimingMode::Inactive:
		break;
	case ETimingMode::PlayerAttack:
		{
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
			CurrentMode = ETimingMode::Inactive;
			bIsTimingActive = false;
			break;
		}
	case ETimingMode::EnemyParry:
		{
			if (bCanParry)
			{
				OnTimingResult.Broadcast(true);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Parry Success"));
			}
			else
			{
				OnTimingResult.Broadcast(false); // 창이 닫혔을 때 누르면 실패
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parry Fail"));
			}
			CurrentMode = ETimingMode::Inactive;
			break;
		}
	default: ;
	}

	// 타이밍 성공/실패 판정

	 // 한 번 판정하면 타이머 중지
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

	if (CurrentMode == ETimingMode::PlayerAttack)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime >= FinishTime)
		{
			// 시간이 다 되면 입력이 없었으므로 실패 처리
			OnTimingResult.Broadcast(false);
			CurrentMode = ETimingMode::Inactive;
		}
	}
}

