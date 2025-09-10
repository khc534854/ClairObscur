// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

#include "AnimNotifyStateParry.h"
#include "EnemyFSM.h"

#include "Enemy.h"

void UEnemyAnimInstance::OnNotifyBegin(UAnimNotifyState* NotifyState)
{
	if (!OwnerEnemy) return;

	// AnimNotifyState 타입 체크
	if (NotifyState->IsA<UAnimNotifyStateParry>())
	{
		OwnerEnemy->fsm->OnParryWindowOpened();
	}
}

void UEnemyAnimInstance::OnNotifyEnd(UAnimNotifyState* NotifyState)
{
	if (!OwnerEnemy) return;

	if (NotifyState->IsA<UAnimNotifyStateParry>())
	{
		OwnerEnemy->fsm->OnParryWindowClosed();
	}
}