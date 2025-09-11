// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerBase.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerPlayer = Cast<APlayerBase>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerPlayer.IsValid())
		OwnerPlayer = Cast<APlayerBase>(TryGetPawnOwner());
	if (!OwnerPlayer.IsValid()) return;

	// FSM 상태 복사 (ABP에서 사용하기 위함)
	AnimFsmState = OwnerPlayer->fsm->CurrentState;

	bMoveOut = OwnerPlayer->fsm->bMoveOut;
	bReturn = OwnerPlayer->fsm->bReturn;
}
