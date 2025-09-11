// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDirectory/PlayerFSM.h"
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
	
	// 에디터 화면에 현재 상태 출력
	FString stateStr = UEnum::GetValueAsString(CurrentState);
	GEngine->AddOnScreenDebugMessage(0,1,FColor::Cyan, stateStr);
	

	 if (bMoveOut || bReturn)
    {
        MoveT += DeltaTime;
        const float Alpha = FMath::Clamp(MoveT / MoveDuration, 0.f, 1.f);
        const FVector P = FMath::Lerp(MoveStart, MoveTarget, Alpha);
	 	
	 	

	 	
        // 충돌 고려해서 이동
        player->SetActorLocation(P, true);

        // 부드러운 회전(적 쪽 보게)
        const FRotator Look = UKismetMathLibrary::FindLookAtRotation(player->GetActorLocation(), bMoveOut ? MoveTarget : StartLocation);
        const FRotator YawOnly(0.f, Look.Yaw, 0.f);
        player->SetActorRotation( FMath::RInterpTo(player->GetActorRotation(), YawOnly, DeltaTime, 10.f) );

        if (Alpha >= 1.f)
        {
            if (bMoveOut)
            {
                // 도착 → 스킬 몽타주 재생
                bMoveOut = false;
            	bReachedSpotThisRun = true; // 공격지점 도착
            	
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
            	player->SetActorRotation(StartRotation);
            	
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
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

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


// 상태 함수
void UPlayerFSM::CombatIdleState()
{
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Orange, TEXT("CombatIdleState"));
}

void UPlayerFSM::SelectActionState()
{
	
}


void UPlayerFSM::SelectSkillState()
{
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Orange, TEXT("SelectSkillState"));
}


void UPlayerFSM::AttackState()
{
	GEngine->AddOnScreenDebugMessage(4, 1, FColor::Orange, TEXT("AttackState"));
}


void UPlayerFSM::DamagedState()
{
	GEngine->AddOnScreenDebugMessage(5, 1, FColor::Orange, TEXT("DamagedState"));

	// 피격 조건 따라서 dodge, damaged, parry
	SelectDamagedFunction = [this](){if (SelectedAction) (this->*SelectedAction)();};
	if (SelectDamagedFunction) SelectDamagedFunction();
}

void UPlayerFSM::DieState()
{
	GEngine->AddOnScreenDebugMessage(6, 1, FColor::Orange, TEXT("DieState"));

	if (DieMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
		player->Destroy();
	}
}


// 피격
void UPlayerFSM::OnTakeDamage()
{
	if (DamagedMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(DamagedMontage);
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

void UPlayerFSM::OnCounter()
{
	// 쳐내기를 모두 성공하면 카운터가 발생 (무조건)
	if (CounterMontage && player->GetMesh() && player->GetMesh()->GetAnimInstance())
	{
		player->GetMesh()->GetAnimInstance()->Montage_Play(CounterMontage);
	}
}


// 스킬 함수
void UPlayerFSM::ExecuteSkill(const FVector& EnemyLocation, int32 SkillIndex)
{
	const FSkillRow* Row = GetSkillRowByIndex(SkillIndex);
	if (!Row) return;

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

	const float R = Row->Distance;     
	MoveTarget = EnemyLocation + dir * R;
	MoveStart  = StartLocation;

	// 3) 이동 시간 계산 (속도 기반)
	const float dist = FVector::Dist2D(MoveStart, MoveTarget);
	MoveDuration = FMath::Max(KINDA_SMALL_NUMBER, dist / MoveSpeed);
	MoveT = 0.f;

	// 4) 이동 시작 플래그
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

