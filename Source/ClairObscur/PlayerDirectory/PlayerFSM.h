// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFSM.generated.h"


UENUM(BlueprintType)
enum class EControlMode : uint8 { Free, Commanded };

UENUM(BlueprintType)
enum class ECommandedPlayerState : uint8
{
	CombatIdle,
	SelectSkill,
	Attack,
	Damaged,
	Die
};
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
	
	// 컨트롤모드 및 디폴트 상태 설정
	UPROPERTY(BlueprintReadOnly)
	EControlMode ControlMode = EControlMode::Commanded;
	UPROPERTY(BlueprintReadOnly)
	ECommandedPlayerState CurrentState = ECommandedPlayerState::CombatIdle;

	// 컨트롤 모드 및 상태 get, set 함수

	// set 
	void SetControlMode(EControlMode Mode) { ControlMode = Mode; }
	void SetCombatState(ECommandedPlayerState NewState) {
		if (ControlMode == EControlMode::Commanded) {
			CurrentState = NewState;
		}
	}

	// get
	EControlMode GetControlMode() const
	{ return ControlMode; }
	
	ECommandedPlayerState GetCombatState() const
	{ return CurrentState; }

	
	//FSM 상태 함수
	void CombatIdleState();
	void SelectSkillState();
	void AttackState();
	void DamagedState();
	void DieState();

	//피격, 회피, 막기 함수
	
};
