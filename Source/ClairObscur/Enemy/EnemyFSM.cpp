// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerDirectory/PlayerBase.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 소유자 찾기
	me = Cast<AEnemy>(GetOwner());
	// 타겟 찾기
	target = Cast<APlayerBase>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerBase::StaticClass()));
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	switch (CurrentState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 4)
	{
		SetEnemyState(EEnemyState::Move);
		currentTime = 0;
	}
}

void UEnemyFSM::MoveState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 2)
	{
		SetEnemyState(EEnemyState::Attack);
		currentTime = 0;
	}
}

void UEnemyFSM::AttackState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 5)
	{
		SetEnemyState(EEnemyState::Damage);
		currentTime = 0;
	}
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 3)
	{
		SetEnemyState(EEnemyState::Die);
		currentTime = 0;
	}
}

void UEnemyFSM::DieState()
{
}

void UEnemyFSM::OnDamageProcess(FVector hitDirection)
{
}

void UEnemyFSM::SetEnemyState(EEnemyState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;

		// Broadcast immediately on state change if entering Move
		if (me)
		{
			// 현재 상태 출력
			UE_LOG(LogTemp, Log, TEXT("State: %s"), *UEnum::GetValueAsString(CurrentState));

			me->OnEnemyMovement.Broadcast(CurrentState == EEnemyState::Move);
			
			switch (CurrentState)
			{
			case EEnemyState::Idle:
				me->EnemyIdle();
				break;
			case EEnemyState::Move:
				me->EnemyMove(FVector(0));
				break;
			case EEnemyState::Attack:
				me->EnemyAttack();
				break;
			case EEnemyState::Damage:
				me->EnemyDamage();
				break;
			case EEnemyState::Die:
				me->EnemyDie();
				break;
			}
		}
	}
}
