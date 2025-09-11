// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleFSMComponent.h"

#include "BattleTimingComponent.h"


// Sets default values for this component's properties
UBattleFSMComponent::UBattleFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UBattleFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (CurrentState)
	{
	case EBattleState::StartBattle:
		StartBattleState();
		break;
	case EBattleState::SelectAction:
		SelectActionState();
		break;
	case EBattleState::SelectSkill:
		SelectSkillState();
		break;
	case EBattleState::SelectTarget:
		SelectTargetState();
		break;
	case EBattleState::PlayerPlayAction:
		PlayerPlayActionState();
		break;
	case EBattleState::EnemyPlayAction:
		EnemyPlayActionState();
		break;
	case EBattleState::Waiting:
		WaitingState();
		break;
	case EBattleState::EndBattle:
		EndBattleState();
		break;
	case EBattleState::NotBattle:
		NotBattleState();
		break;
	}
}

void UBattleFSMComponent::StartBattleState()
{
}

void UBattleFSMComponent::SelectActionState()
{
}

void UBattleFSMComponent::SelectSkillState()
{
}

void UBattleFSMComponent::SelectTargetState()
{
}

void UBattleFSMComponent::PlayerPlayActionState()
{
}

void UBattleFSMComponent::EnemyPlayActionState()
{
}

void UBattleFSMComponent::WaitingState()
{
}

void UBattleFSMComponent::EndBattleState()
{
}

void UBattleFSMComponent::NotBattleState()
{
}

void UBattleFSMComponent::ChangeState(EBattleState newState)
{
	BeforeState = CurrentState;
	CurrentState = newState;
	OnStateChanged.Broadcast(CurrentState);
}

EBattleState UBattleFSMComponent::GetCurrentState()
{
	return CurrentState;
}

EBattleState UBattleFSMComponent::GetBeforeState()
{
	return BeforeState;
}

