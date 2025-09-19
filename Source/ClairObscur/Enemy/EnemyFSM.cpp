// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "Enemy.h"
#include "EnemyAnimInstance.h"
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
}

void UEnemyFSM::MoveState()
{
	if (bCounterAttackIng)
		return;
	
	// targetVector 로 이동을 시키자 1 : target한테 2 :원위치로
	// 공격 조건 되면 move
	if (!didIAttack)
	{
		if ((targetVector - me->GetActorLocation()).Size() < distanceTarget)
		{
			SetEnemyState(EEnemyState::Attack);
			return;
		}
		else
		{
			me->SetActorLocation(me->GetActorLocation() + moveSpeed * moveDirection * GetWorld()->GetDeltaSeconds());

		}
	}
	else
	{
		if ((me->GetActorLocation()-enemyOriginLocation).Size() > 10)
		{
			me->SetActorLocation(me->GetActorLocation() + moveSpeed * returnDirection * GetWorld()->GetDeltaSeconds());
		}
		else
		{
			didIAttack = false;
			SetEnemyState(EEnemyState::Idle);
			OnEnemyActionFinished.Broadcast();
		}
	}


}

void UEnemyFSM::AttackState()
{
}

void UEnemyFSM::DamageState()
{
}

void UEnemyFSM::DieState()
{
}

void UEnemyFSM::OnDamageProcess(FVector hitDirection)
{
}

void UEnemyFSM::SetTargetToMove(FVector targetVect)
{
	targetVector = targetVect;
}



void UEnemyFSM::SetEnemyState(EEnemyState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		me->AnimInst->animState = CurrentState;

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
				//공경전 무브면 내위치오리진 저장
				if (!didIAttack)
				{
					enemyOriginLocation = me->GetActorLocation();
					me->skillIndex = FMath::RandRange(2,2);
				}
				moveDirection = (targetVector - me->GetActorLocation()).GetSafeNormal();
				returnDirection = (enemyOriginLocation - me->GetActorLocation()).GetSafeNormal();
				me->EnemyMove();
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

void UEnemyFSM::OnParryWindowOpened()
{
	bCanBeParried = true;  // 파리 가능 상태 시작
}

void UEnemyFSM::OnParryWindowClosed()
{
	bCanBeParried = false; // 파리 가능 상태 종료
}

EEnemyState UEnemyFSM::GetState()
{
	return CurrentState;
}
