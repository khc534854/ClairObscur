// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAnimInstance.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


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

	FVector velocity = OwnerPlayer->GetVelocity();
	FVector forward = OwnerPlayer->GetActorForwardVector();
	FVector right = OwnerPlayer ->GetActorRightVector();

	speed = UKismetMathLibrary::Dot_VectorVector(velocity, forward);
	direction = FVector::DotProduct(velocity, right);
	auto* cmp = OwnerPlayer->GetCharacterMovement();
	isInAir = cmp->IsFalling();

	
	// FSM 상태 복사 (ABP에서 사용하기 위함)
	AnimFsmState = OwnerPlayer->fsm->CurrentState;

	bMoveOut = OwnerPlayer->fsm->bMoveOut;
	bReturn = OwnerPlayer->fsm->bReturn;

	// 그래플링
	bGrappling  = OwnerPlayer->bGrappling;
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

	// 구스타브 카운터
	if (NotifyName == "Counter_Hit" && NS_GustavCounterAttackSystem)
	{
		NS_GustavCounterAttackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_GustavCounterAttackSystem,
			OwnerChar->fsm->EnemyLoc,  // 에너미 로케이션에 ns 붙이기
			FRotator::ZeroRotator,
			FVector(3)
		);

	}
	

	if (NotifyName == "LuneBasic_Hit" && NS_LuneBasicAttackSystem)
	{
		NS_LuneBasicAttackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_LuneBasicAttackSystem,
			OwnerChar->fsm->EnemyLoc, // 에너미 로케이션에 ns 붙이기
			FRotator::ZeroRotator, 
			FVector(2)
	
		);

	}

	
	if (NotifyName == "LuneOne_Hit" && NS_LuneOneAttackSystem)
	{
		NS_LuneOneAttackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_LuneOneAttackSystem,
			OwnerChar->fsm->EnemyLoc,  // 에너미 로케이션에 ns 붙이기
			FRotator::ZeroRotator, 
			FVector(2)
	);

	}

	if (NotifyName == "LuneIce_Hit" && NS_LuneIceAttackSystem)
	{
		NS_LuneIceAttackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_LuneIceAttackSystem,
			OwnerChar->fsm->EnemyLoc, // 에너미 로케이션에 ns 붙이기
			FRotator::ZeroRotator, 
				FVector(1)
		);

	}

	if (NotifyName == "LuneCounter_Hit" && NS_LuneCounterAttackSystem)
	{
		NS_LuneCounterAttackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_LuneCounterAttackSystem,
			OwnerChar->fsm->EnemyLoc,
			FRotator::ZeroRotator, 
			FVector(1)
		);

	}
	

	
}


