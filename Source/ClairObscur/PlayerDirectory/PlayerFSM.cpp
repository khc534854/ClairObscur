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
}


// 상태 함수
void UPlayerFSM::CombatIdleState()
{
}


void UPlayerFSM::SelectSkillState()
{
}

void UPlayerFSM::AttackState()
{
}

void UPlayerFSM::DamagedState()
{
}

void UPlayerFSM::DieState()
{
}

