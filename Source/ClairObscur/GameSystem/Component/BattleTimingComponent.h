// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleTimingComponent.generated.h"

UENUM()
enum class ETimingMode : uint8
{
	Inactive,       // 비활성 상태
	PlayerAttack,   // 플레이어 공격 QTE
	EnemyParry      // 적 공격 패링/회피
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimingResultSignature, bool, bSuccess, ETimingMode, TimingMode);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UBattleTimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBattleTimingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void StartTimingEvent(float Duration, float SuccessWindowStart, float SuccessWindowEnd);

	void StartParryTiming();
	void EndParryTiming();

	void OnPlayerInput();
	
	// 외부(UI)에서 현재 진행률을 알기 위한 함수
	float GetCurrentTimingPercent() const;

	
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnTimingResultSignature OnTimingResult;
	bool bCanParry = false;

	bool bIsTimingActive = false;
private:
	ETimingMode CurrentMode = ETimingMode::Inactive;
	float CurrentTime = 0.f;
	float FinishTime = 1.f;
	float SuccessStart = 0.f;
	float SuccessEnd = 0.f;
};
