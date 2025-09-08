// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDirectory/PlayerFSM.h"
#include "PlayerDirectory/PlayerBase.h"


// Sets default values for this component's properties
UPlayerFSM::UPlayerFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerFSM::BeginPlay()
{
	Super::BeginPlay();

	// 컴포넌트 소유자 (플레이어) 찾기
	player = Cast<APlayerBase>(GetOwner());
	
	// ...
	
}


// Called every frame
void UPlayerFSM::TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	// 상태 변경
	switch (CurrentState)
	{
	case ECommandedPlayerState::CombatIdle:
		CombatIdleState();
		break;
	case ECommandedPlayerState::SelectSkill:
		SelectSkillState();
		break;
	case ECommandedPlayerState::Attack:
		AttackState();
		break;
	case ECommandedPlayerState::Damaged:
		DamagedState();
		break;
	case ECommandedPlayerState::Die:
		DieState();
		break;
	}
	
	// 에디터 화면에 현재 상태 출력
	FString stateStr = UEnum::GetValueAsString(CurrentState);
	GEngine->AddOnScreenDebugMessage(0,1,FColor::Cyan, stateStr);

	FString modeStr = UEnum::GetValueAsString(ControlMode);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Black, modeStr);
	
}

// 상태 함수
void UPlayerFSM::CombatIdleState()
{
	if (player->IsFreeControl()) { return; }
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Orange, TEXT("CombatIdleState"));
}

void UPlayerFSM::SelectSkillState()
{
	if (player->IsFreeControl()) { return; }
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Orange, TEXT("SelectSkillState"));

}

void UPlayerFSM::AttackState()
{
	if (player->IsFreeControl()) { return; }
	GEngine->AddOnScreenDebugMessage(4, 1, FColor::Orange, TEXT("AttackState"));

}

void UPlayerFSM::DamagedState()
{
	if (player->IsFreeControl()) { return; }
	GEngine->AddOnScreenDebugMessage(5, 1, FColor::Orange, TEXT("DamagedState"));

}

void UPlayerFSM::DieState()
{
	if (player->IsFreeControl()) { return; }
	GEngine->AddOnScreenDebugMessage(6, 1, FColor::Orange, TEXT("DieState"));
}


// 피격
void UPlayerFSM::OnTakeDamage()
{
	if (player->IsFreeControl()) { return; }
}

// 회피
void UPlayerFSM::OnDodge()
{
	if (player->IsFreeControl()) { return; }
}

// 쳐내기
void UPlayerFSM::OnParry()
{
	if (player->IsFreeControl()) { return; }
}




