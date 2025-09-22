// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleResultDataComponent.generated.h"

USTRUCT(BlueprintType)
struct FBattleResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 MaxHitDamage = 0;			// 최고 데미지
	UPROPERTY(BlueprintReadOnly) int32 TotalDamage = 0;			// 내가 준 총 누적 데미지
	UPROPERTY(BlueprintReadOnly) int32 TotalDamageReceived = 0;   // 내가 받은 총 누적 피해
	UPROPERTY(BlueprintReadOnly) float BattleSeconds = 0.f;			
	UPROPERTY(BlueprintReadOnly) int32 SuccessParryCount = 0;
	UPROPERTY(BlueprintReadOnly) int32 SuccessDodgeCount = 0;
};



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UBattleResultDataComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:
	// Sets default values for this component's properties
	UBattleResultDataComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction*
	                           ThisTickFunction) override;

	double StartTimeSeconds = 0.0;
	FBattleResult Current;  // 포인터(X), Subobject(X) — 그냥 값!

public:
	UFUNCTION(BlueprintCallable)
	void StartBattle();
	UFUNCTION(BlueprintCallable)
	FBattleResult EndBattle(); // 전투 결과 반환
	bool bActive = false;

	
	// 가한 데미지
	UFUNCTION(BlueprintCallable)
	void RecordDamage(int32 damage);
	
	// 피격 데미지
	UFUNCTION(BlueprintCallable)
	void RecordDamageReceived(int32 damage);

	// 성공 패리 수
	UFUNCTION(BlueprintCallable)
	void ParrySuccess();

	// 성공 닷지 수
	UFUNCTION(BlueprintCallable)
	void DodgeSuccess();
};
