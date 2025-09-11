// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

#include "AnimNotifyStateParry.h"
#include "EnemyFSM.h"

#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* pawnOwner = TryGetPawnOwner();
	if (pawnOwner)
	{
		ownerEnemy = Cast<AEnemy>(pawnOwner);
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ownerEnemy) return;

	/*// 이동 속도
	FVector Velocity = ownerEnemy->GetVelocity();
	ownerEnemy = Velocity.Size();

	// FSM 상태 기반 플래그 (예시)
	bIsAttacking = ownerEnemy->fsm && ownerEnemy->fsm->IsInAttackState();
	bIsDead      = ownerEnemy->fsm && ownerEnemy->fsm->IsInDeadState();*/
}