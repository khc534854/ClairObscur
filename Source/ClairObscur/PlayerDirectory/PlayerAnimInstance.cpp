// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAnimInstance.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerBase.h"



UPlayerAnimInstance::UPlayerAnimInstance()
{
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempNS(TEXT("/Script/Niagara.NiagaraSystem'/Game/Game/Player/Asset/FX/NS_Demon_Slash.NS_Demon_Slash'"));

	if (TempNS.Succeeded())
	{
		NS_OverChargeSystem = TempNS.Object;
	}

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempNS_sword(TEXT("/Script/Niagara.NiagaraSystem'/Game/SlashHitVFX/NS/NS_Slash_Reaper.NS_Slash_Reaper'"));

	if (TempNS_sword.Succeeded())
	{
		NS_SwordAttackSystem = TempNS_sword.Object;
	}
}



void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPlayer = Cast<APlayerBase>(TryGetPawnOwner());

	// anim notify begin  
	OnPlayMontageNotifyBegin.AddDynamic(this, &UPlayerAnimInstance::OnNotifyBegin);
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



void UPlayerAnimInstance::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	// 소유 캐릭터 찾기
	APlayerBase* OwnerChar = Cast<APlayerBase>(TryGetPawnOwner());
	if (!OwnerChar) return;
	
	if (NotifyName == "OverCharge_Hit" && NS_OverChargeSystem)
	{
		NS_OverChargeComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_OverChargeSystem,
			OwnerChar->fsm->EnemyLoc  // 에너미 로케이션에 ns 붙이기
		);

	}

	if (NotifyName == "Combo_Hit" && NS_SwordAttackSystem)
	{
		NS_SwordAttackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_SwordAttackSystem,
			OwnerChar->fsm->EnemyLoc  // 에너미 로케이션에 ns 붙이기
		);

	}

	
}


