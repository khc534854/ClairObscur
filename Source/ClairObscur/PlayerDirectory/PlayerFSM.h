// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterComponent/SkillRow.h"
#include "Animation/AnimInstance.h" 
#include "PlayerFSM.generated.h"


struct FSkillRow;


UENUM(BlueprintType)
enum class ECommandedPlayerState : uint8
{
	CombatIdle,
	SelectAction,
	SelectSkill,
	Attack,
	Damaged,
	Die
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnSkillSequenceCompleted,
	int32, SkillIndex,
	bool,  bInterrupted,
	bool,  bReachedAttackSpot
);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UPlayerFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction*
	                           ThisTickFunction) override;

	// 컴포넌트 소유자
	UPROPERTY(EditAnywhere)
	class APlayerBase* player;
	
	// 디폴트 상태 설정
	UPROPERTY(BlueprintReadOnly)
	ECommandedPlayerState CurrentState = ECommandedPlayerState::CombatIdle;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInCombat = false;
	
	// 전투 모드 전환
	UFUNCTION(BlueprintCallable)
	void EnterCombatMode();

	UFUNCTION(BlueprintCallable)
	void ExitCombatMode();
	
	// 상태 get, set 함수
	
	// set
	UFUNCTION(BlueprintCallable)
	void SetCommandedState(ECommandedPlayerState NewState)
	{ CurrentState = NewState;}

	// get
	UFUNCTION(BlueprintCallable)
	ECommandedPlayerState GetCommandedState() const
	{ return CurrentState; }
	
	
	//FSM 상태 함수
	UFUNCTION(BlueprintCallable, Category = "State")
	void CombatIdleState();
	UFUNCTION(BlueprintCallable, Category = "State")
	void SelectActionState();
	UFUNCTION(BlueprintCallable, Category = "State")
	void SelectSkillState();
	UFUNCTION(BlueprintCallable, Category = "State")
	void AttackState();
	UFUNCTION(BlueprintCallable, Category = "State")
	void DamagedState();
	UFUNCTION(BlueprintCallable, Category = "State")
	void DieState();


	//피격, 회피, 쳐내기(막기)  함수
	// 함수들 담겨있는 배열
	TFunction<void()> SelectDamagedFunction;

	// 선택된 함수 포인터
	using FFSMAction = void (UPlayerFSM::*)();
	FFSMAction       SelectedAction = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void OnTakeDamage();
	UFUNCTION(BlueprintCallable, Category = "State")
	void OnDodge();
	UFUNCTION(BlueprintCallable, Category = "State")
	void OnParry();
	
	//카운터
	UFUNCTION(BlueprintCallable, Category = "State")
	void OnCounter();

	
	// 스킬 데이터 테이블
	UPROPERTY(EditDefaultsOnly, Category="SKill")
	TSoftObjectPtr<UDataTable> SkillTable;

	// 스킬 실행 함수
	UFUNCTION(BlueprintCallable, Category = "SKill")
	void ExecuteSkill(const FVector& EnemyLocation, int32 SkillIndex);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	const FSkillRow* GetSkillRowByIndex(int32 Index) const;

	UPROPERTY(BlueprintAssignable, Category="Skill")
	FOnSkillSequenceCompleted OnSkillSequenceCompleted;


	
	// 이번 시퀀스에서 공격 지점까지 실제 도달했는지
	bool bReachedSpotThisRun = false;

	// 마지막 몽타주가 중단됐는지
	bool bLastMontageInterrupted = false;
	
	// 보간 상태
	FVector MoveStart, MoveTarget, StartLocation;
	FRotator StartRotation;
	float   MoveT = 0.f;            // 진행 시간
	float   MoveDuration = 0.f;     // 총 이동 시간
	float   MoveSpeed = 1000.f;      // uu/s

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	bool    bMoveOut = false;       // 적에게 가는 중

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	bool    bReturn  = false;       // 돌아오는 중
	int32   PendingSkillIndex = INDEX_NONE; // 도착 후 쓸 스킬

	// ABP
	UPROPERTY(EditAnywhere, Category="Animation")
	TSubclassOf<UAnimInstance> CombatAnimClass;

	UPROPERTY(EditAnywhere, Category="Animation")
	TSubclassOf<UAnimInstance> FreeAnimClass;
	

	

	// 몽타주
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* IntroMontage; 

	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* StartAttackMontage;
	
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* DamagedMontage;
	
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* DodgeMontage;
	
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* ParryMontage;

	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* CounterMontage;
	
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* DieMontage;
};
