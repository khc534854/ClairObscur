// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDirectory/PlayerFSM.h"

#include "PlayerCameraShake.h"
#include "CharacterComponent/SkillRow.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerDirectory/PlayerBase.h"


// Sets default values for this component's properties
UPlayerFSM::UPlayerFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlayerFSM::BeginPlay()
{
	Super::BeginPlay();
	

	// 컴포넌트 소유자 (플레이어) 찾기
	player = Cast<APlayerBase>(GetOwner());
	
	// 1. 몽타주 사전 로딩
	if (IntroMontage)   IntroMontage->AddToRoot();
	if (DamagedMontage) DamagedMontage->AddToRoot();
	if (DodgeMontage)   DodgeMontage->AddToRoot();
	if (ParryMontage)   ParryMontage->AddToRoot();
	if (DieMontage)     DieMontage->AddToRoot();
	
	// 2. 스킬 테이블 몽타주 전부 프리로드
	warmup();  
	
}


// Called every frame
void UPlayerFSM::TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

	// ...

	if (!bIsInCombat) {return;}
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
	

	 if (bMoveOut || bReturn)
    {
        MoveT += DeltaTime;
        const float Alpha = FMath::Clamp(MoveT / MoveDuration, 0.f, 1.f);
        const FVector P = FMath::Lerp(MoveStart, MoveTarget, Alpha);
	 	
        // 충돌 고려해서 이동
        player->SetActorLocation(P, true);

        // 부드러운 회전(적 쪽 보게)
        //const FRotator Look = UKismetMathLibrary::FindLookAtRotation(player->GetActorLocation(), bMoveOut ? MoveTarget : StartLocation);
        //const FRotator YawOnly(0.f, Look.Yaw, 0.f);
        //player->SetActorRotation( FMath::RInterpTo(player->GetActorRotation(), YawOnly, DeltaTime, 10.f) );

	 	FVector Dir = (EnemyLoc - player->GetActorLocation()).GetSafeNormal();

	 	// LookAt 회전 계산
	 	FRotator LookAtRot = Dir.Rotation();

	 	// 플레이어 회전 적용
	 	FRotator YawOnly(0.f, LookAtRot.Yaw, 0.f);
	 	player->SetActorRotation(YawOnly);


        if (Alpha >= 1.f)
        {
            if (bMoveOut)
            {
                // 도착 → 스킬 몽타주 재생
                bMoveOut = false;
            	bReachedSpotThisRun = true; // 공격지점 도착
            	
            	// 플레이어 회전 변경
            	player->SetActorRotation(YawOnly);

                if (const FSkillRow* Row = GetSkillRowByIndex(PendingSkillIndex))
                {
                    if (UAnimMontage* M = Row->SkillMontage.LoadSynchronous())
                    {
                        if (UAnimInstance* Anim = player->GetMesh()->GetAnimInstance())
                        {
                            // 끝나면 복귀 시작
                            Anim->OnMontageEnded.AddDynamic(this, &UPlayerFSM::OnMontageEnded);
                            Anim->Montage_Play(M);
                            return; // 복귀는 몽타주 끝나고
                        }
                    }
                }
                // 몽타주가 없거나 실패하면 즉시 복귀 세팅
                MoveStart   = player->GetActorLocation();
                MoveTarget  = StartLocation;
                MoveDuration= FMath::Max(KINDA_SMALL_NUMBER, FVector::Dist2D(MoveStart, MoveTarget) / MoveSpeed);
                MoveT = 0.f;
                bReturn = true;
            }
            else if (bReturn)
            {
                // 복귀 완료
                bReturn = false;
          
        		// 플레이어 회전 변경
            	player->SetActorRotation(YawOnly);

            	//게임시스템에 "공격 종료" 콜백
            	OnSkillSequenceCompleted.Broadcast(
                PendingSkillIndex,
                bLastMontageInterrupted,
                bReachedSpotThisRun);

            	// 상태 정리
				PendingSkillIndex       = INDEX_NONE;
            	bReachedSpotThisRun     = false;
            	bLastMontageInterrupted = false;
            
            }
        }
    }
}

void UPlayerFSM::EnterCombatMode()
{
	
	bIsInCombat = true;
	USkeletalMeshComponent* Mesh = player->GetMesh();
	//Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// 전투모드 ABP로 변경
	if (CombatAnimClass)
	{
		Mesh->SetAnimInstanceClass(CombatAnimClass);
	}
	
	// 전투 모드 처음 들어가면 나오는 애니메이션
	if (IntroMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(IntroMontage);
	}
	
	// 무기 스폰
	player->SpawnWeapon();

	// HP, AP 셋팅
	player->currentHP = player->maxHP;
	player->currentAP = 6;
	player->setplayerHP(0, nullptr);
	player->setplayerAP(0);

	UE_LOG(LogTemp, Warning, TEXT("AP: %d, HP: %d"), player->currentAP, player->currentHP);

}

void UPlayerFSM::ExitCombatMode()
{
	bIsInCombat = false;
	
	USkeletalMeshComponent* Mesh = player->GetMesh();
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
	// 맵이동 ABP로 변경
	if (FreeAnimClass)
	{
		Mesh->SetAnimInstanceClass(FreeAnimClass);
	}
	
	// 무기 삭제
    player->DestroyWeapon();
    	
}

void UPlayerFSM::SetCommandedState(ECommandedPlayerState NewState)
{
	if (CurrentState == ECommandedPlayerState::Die)
		return;
	
	CurrentState = NewState;
	if (NewState == ECommandedPlayerState::Die)
	{
		if (DieMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
		{
			player->GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
		}
	}
}


// 상태 함수
void UPlayerFSM::CombatIdleState()
{
	//GEngine->AddOnScreenDebugMessage(2, 1, FColor::Orange, TEXT("CombatIdleState"));
}

void UPlayerFSM::SelectActionState()
{
	
}


void UPlayerFSM::SelectSkillState()
{
	//GEngine->AddOnScreenDebugMessage(3, 1, FColor::Orange, TEXT("SelectSkillState"));
}


void UPlayerFSM::AttackState()
{
	//GEngine->AddOnScreenDebugMessage(4, 1, FColor::Orange, TEXT("AttackState"));
}


void UPlayerFSM::DamagedState()
{
	//GEngine->AddOnScreenDebugMessage(5, 1, FColor::Orange, TEXT("DamagedState"));

	// 피격 조건 따라서 dodge, damaged, parry
	SelectDamagedFunction = [this](){if (SelectedAction) (this->*SelectedAction)();};
	if (SelectDamagedFunction) SelectDamagedFunction();


	
}

void UPlayerFSM::DieState()
{
	//GEngine->AddOnScreenDebugMessage(6, 1, FColor::Orange, TEXT("DieState"));

	// if (DieMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	// {
	// 	player->GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
	// 	
	// 	//player->Destroy();
	// }
}


// 피격
void UPlayerFSM::OnTakeDamage()
{
	if (DamagedMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(DamagedMontage);
	}
	
	// 히트스탑 적용
	ApplyHitStopFromSkill();

	// 카메라 흔들림
	APlayerController* PC = Cast<APlayerController>(player->GetController());
	if (!PC){UE_LOG(LogTemp, Warning, TEXT("PC NULL"))};
	
	if (PC)
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(UPlayerCameraShake::StaticClass(), 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("CS Call")); 
		}
	}
	
}

// 회피
void UPlayerFSM::OnDodge()
{
	if (DodgeMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(DodgeMontage);
	}
}

// 쳐내기
void UPlayerFSM::OnParry()
{
	if (ParryMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(ParryMontage);
	}
}


// 스킬 함수
void UPlayerFSM::ExecuteSkill(const FVector& EnemyLocation, int32 SkillIndex)
{
	const FSkillRow* Row = GetSkillRowByIndex(SkillIndex);
	if (!Row) return;

	// 사용 AP 감소시키기
	player->setplayerAP(Row->AP);

	// 완료 이벤트용 상태 초기화
	PendingSkillIndex       = SkillIndex;
	bReachedSpotThisRun     = false;
	bLastMontageInterrupted = false;
	
	EnemyLoc = EnemyLocation;
	
	// 1) 시작 상태 저장
	StartLocation  = player->GetActorLocation();
	StartRotation  = player->GetActorRotation();
	PendingSkillIndex = SkillIndex;

	// 2) 목표점 계산 (적→플레이어 선상에서 Row->Distance 지점)
	//  방향 수정: Enemy + normalize( Start - Enemy ) * R
	FVector dir = (StartLocation - EnemyLocation);
	dir.Z = 0; 
	if (!dir.Normalize()) dir = FVector::ForwardVector;

	// 적과의 거리 계산
	float TotalDist = FVector::Dist2D(StartLocation, EnemyLocation);
	float Ratio = FMath::Clamp(Row->Distance, 0.f, 1.f);
	MoveTarget = EnemyLocation + dir * (TotalDist * Ratio);
	MoveTarget.Z = StartLocation.Z;
	MoveStart  = StartLocation;
	
	// 3) 이동 시간 계산 (속도 기반)
	const float dist = FVector::Dist2D(MoveStart, MoveTarget);
	MoveDuration = FMath::Max(KINDA_SMALL_NUMBER, dist / MoveSpeed);
	MoveT = 0.f;

	// 4) 플레이어 방향 변경 (적 쪽 보게)
	FVector Dir = (EnemyLoc - player->GetActorLocation()).GetSafeNormal();

	// LookAt 회전 계산
	FRotator LookAtRot = Dir.Rotation();

	// 플레이어 회전 적용
	FRotator YawOnly(0.f, LookAtRot.Yaw, 0.f);
	player->SetActorRotation(YawOnly);
	
	
	// 5) 이동 시작 플래그
	bReturn  = false;
	bMoveOut = true;
}


void UPlayerFSM::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) 
{
	if (UAnimInstance* Anim = player->GetMesh()->GetAnimInstance())
	Anim->OnMontageEnded.RemoveDynamic(this, &UPlayerFSM::OnMontageEnded);

	bLastMontageInterrupted = bInterrupted;
	
	MoveStart   = player->GetActorLocation();
	MoveTarget  = StartLocation;
	MoveDuration= FMath::Max(KINDA_SMALL_NUMBER, FVector::Dist2D(MoveStart, MoveTarget) / MoveSpeed);
	MoveT = 0.f;
	bReturn = true;
}



const FSkillRow* UPlayerFSM::GetSkillRowByIndex(int32 Index) const
{
	UDataTable* Table = SkillTable.LoadSynchronous();
	if (!Table){return nullptr;}

	// 매 호출마다 RowNames 가져오기 
	const TArray<FName> RowNames = Table->GetRowNames();
	if (!RowNames.IsValidIndex(Index)) {return nullptr;}

	static const FString Ctx = TEXT("GetSkillRowByIndex");
	return Table->FindRow<FSkillRow>(RowNames[Index], Ctx);
}



// 히트 스탑 
void UPlayerFSM::ApplyHitStopFromSkill()
{
	/*
	// 스킬 데이터 가져오기
	const FSkillRow* Row = GetSkillRowByIndex(Index);
	if (!Row)
	{
		return;
	}

	// 1) Custom Time Dilation 적용
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->CustomTimeDilation = FMath::Clamp(Row->HitStopDilation, 0.01f, 10.f);
	}

	// 2) 타이머로 복구
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitStopTimerHandle);
		World->GetTimerManager().SetTimer(
			HitStopTimerHandle,
			this, &UPlayerFSM::ResetTimeDilation,
			Row->HitStopDuration,
			false
		);
	}
	*/


	UE_LOG(LogTemp, Warning, TEXT("Hit stop call")); 
	float UniformHitStopDilation = 0.1f;
	float UniformHitStopDuration = 0.08f;



	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->CustomTimeDilation = FMath::Clamp(UniformHitStopDilation, 0.01f, 10.f);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitStopTimerHandle);
		World->GetTimerManager().SetTimer(
			HitStopTimerHandle,
			this, &UPlayerFSM::ResetTimeDilation,
			FMath::Max(0.f, UniformHitStopDuration),
			false
		);
	}
	
}


// 원래대로 복구
void UPlayerFSM::ResetTimeDilation()
{
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->CustomTimeDilation = 1.0f;
	}
}



void UPlayerFSM::warmup()
{
	//프리로드

	// Skill Table 미리 로딩
	UDataTable* Table = SkillTable.LoadSynchronous();
	if (!Table) return;

	static const FString Ctx = TEXT("Warmup_SkillMontages");
	const TArray<FName> RowNames = Table->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		if (const FSkillRow* Row = Table->FindRow<FSkillRow>(RowName, Ctx))
		{
			if (Row->SkillMontage.IsPending())
			{
				UAnimMontage* M = Row->SkillMontage.LoadSynchronous();
				if (M) M->AddToRoot();
			}
		}
	}
	
}

