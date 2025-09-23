// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleResultDataComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UBattleResultDataComponent::UBattleResultDataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleResultDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBattleResultDataComponent::TickComponent(float DeltaTime,
                                               ELevelTick TickType,
                                               FActorComponentTickFunction*
                                               ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



void UBattleResultDataComponent::StartBattle()  // 전투 시작 시 호출됨
{
	UE_LOG(LogTemp, Warning, TEXT("StartBattle"));
	Current = FBattleResult{}; // 값 초기화
	bActive = true; 
	StartTimeSeconds = UGameplayStatics::GetTimeSeconds(GetWorld());
	
}

// 타이밍에 맞춰서 호출하기
FBattleResult UBattleResultDataComponent::EndBattle()  // 전투 종료 시 모든 데이터 ui에 전달
{
	// 모든 데이터 전달해주기
	UE_LOG(LogTemp, Warning, TEXT("EndBattle"));
	if (bActive)
	{
		const double Now = UGameplayStatics::GetTimeSeconds(GetWorld());
		Current.BattleSeconds = static_cast<float>(Now - StartTimeSeconds);
		bActive = false;
	}
	return Current; 
}

void UBattleResultDataComponent::RecordDamage(int32 damage) // 적 공격 성공시 호출
{
	if (!bActive) {return;}
	Current.TotalDamage += damage;

	// 최고 데미지 갱신
	if (damage > Current.MaxHitDamage)
	{
		Current.MaxHitDamage = damage;
	}
}

void UBattleResultDataComponent::RecordDamageReceived(int32 damage) // 피격 당했을 때 호출
{
	// 누적 피격 데미지 계산
	if (!bActive) {return;}
	Current.TotalDamageReceived += damage;
	
}

void UBattleResultDataComponent::ParrySuccess() // 패리 성공시 호출
{
	UE_LOG(LogTemp, Warning, TEXT("ParrySuccess"));
	if (!bActive) {return;}
	Current.SuccessParryCount += 1;
}

void UBattleResultDataComponent::DodgeSuccess() // 닷지 성공 시 호출
{
	UE_LOG(LogTemp, Warning, TEXT("DodgeSuccess"));
	if (!bActive) {return;}
	Current.SuccessDodgeCount += 1;
}

