// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTimingComponent.h"

#include "BattleFSMComponent.h"
#include "BattleResultDataComponent.h"
#include "BattleTurnComponent.h"
#include "BattleUIComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameSystem/BattleManager.h"
#include "GameSystem/Widget/WidgetComponent/DamagePopupComponent.h"
#include "GameSystem/Widget/WidgetComponent/QTEWidget.h"
#include "PlayerDirectory/PlayerBase.h"


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

	auto timingUI = Cast<ABattleManager>(GetOwner())->BattleUIComp->TimingWidget;
	if (timingUI)
	{
		timingUI->PlayAnimation(Cast<UQTEWidget>(timingUI)->Timing);
	}
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
		OnTimingResult.Broadcast(false, ETimingMode::EnemyParry); // 창이 닫혔을 때 누르면 실패
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parry Fail"));
		bIsTimingActive = false;
	}
	CurrentMode = ETimingMode::Inactive;
	bCanParry = false;
}

void UBattleTimingComponent::OnPlayerInput()
{
	if (!bIsTimingActive) return;
	
	auto* bm = Cast<ABattleManager>(GetOwner());
	
	switch (CurrentMode)
	{
	case ETimingMode::Inactive:
		break;
	case ETimingMode::PlayerAttack:
		{
			auto* player = Cast<APlayerBase>(bm->BattleTurnComp->GetCurrentTurnCharacter());
			
			if (CurrentTime >= SuccessStart && CurrentTime <= SuccessEnd)
			{ 
				OnTimingResult.Broadcast(true, ETimingMode::PlayerAttack); // 성공 방송
				player->damageComp->SpawnDodgeTypeAt(player->GetActorLocation(), TEXT("PERFECT"));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Timing Success"));

				// 전투 결과 업데이트

			}
			else
			{
				OnTimingResult.Broadcast(false, ETimingMode::PlayerAttack); // 실패 방송
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timing Fail"));
			}
			CurrentMode = ETimingMode::Inactive;
			bIsTimingActive = false;
			break;
		}
	case ETimingMode::EnemyParry:
		{
			auto* player = Cast<APlayerBase>(bm->PlayerParty[bm->EnemyTargetIndex]);
			if (bCanParry)
			{
				OnTimingResult.Broadcast(true, ETimingMode::EnemyParry);
				CurrentParryCount++;
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Parry Success"));

				// PARRY SUCCESS UI


				player->damageComp->SpawnDodgeTypeAt(player->GetActorLocation(), TEXT("PARRIED"));// dodge면 "PERFECT"로 넣어줘야 함.
				
				//counter
				if (CurrentParryCount == MaxAttackCount)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("All Parry Success"));
					FTimerHandle TempTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TempTimerHandle, [this]() {
						auto tempBattleFSMComp = Cast<ABattleManager>(GetOwner())->BattleFSMComp;
						if (tempBattleFSMComp)
						{
							tempBattleFSMComp->ChangeState(EBattleState::Waiting);
						}
					}, 1.f, false);
					
					//Cast<ABattleManager>(GetOwner())->BattleFSMComp->ChangeState(EBattleState::Waiting);
				}
				bm->BattleResultComp->ParrySuccess();
			}
			else
			{
				OnTimingResult.Broadcast(false, ETimingMode::EnemyParry); // 창이 닫혔을 때 누르면 실패
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parry Fail"));
			}
			CurrentMode = ETimingMode::Inactive;
			break;
		}
	default: ;
	}
}

void UBattleTimingComponent::OnPlayerInputQ()
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
				OnTimingResult.Broadcast(true, ETimingMode::PlayerAttack); // 성공 방송
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Timing Success"));
			}
			else
			{
				OnTimingResult.Broadcast(false, ETimingMode::PlayerAttack); // 실패 방송
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
				OnTimingResult.Broadcast(true, ETimingMode::EnemyParry);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Dodge Success"));

				// PARRY SUCCESS UI
				auto* bm = Cast<ABattleManager>(GetOwner());
				auto* player = Cast<APlayerBase>(bm->PlayerParty[bm->EnemyTargetIndex]);
				player->damageComp->SpawnDodgeTypeAt(player->GetActorLocation(), TEXT("PERFECT"));// dodge면 "PERFECT"로 넣어줘야 함.

				// 전투 결과 업데이트
				bm->BattleResultComp->DodgeSuccess();

			}
			else
			{
				OnTimingResult.Broadcast(false, ETimingMode::EnemyParry); // 창이 닫혔을 때 누르면 실패
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parry Fail"));
			}
			CurrentMode = ETimingMode::Inactive;
			break;
		}
	default: ;
	}
}

float UBattleTimingComponent::GetCurrentTimingPercent() const
{
	if (FinishTime <= 0.f) return 0.f;
	return FMath::Clamp(CurrentTime / FinishTime, 0.f, 1.f);
}
//
// void UBattleTimingComponent::StartEnemyParrySequence(int32 NumberOfHits)
// {
// }


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
			OnTimingResult.Broadcast(false, ETimingMode::PlayerAttack);
			CurrentMode = ETimingMode::Inactive;
		}
	}
}

