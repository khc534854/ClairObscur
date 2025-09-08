// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleFSMComponent.generated.h"

UENUM()
enum class EBattleState
{
	     StartBattle,
	    SelectAction,
	     SelectSkill,
	    SelectTarget,
	PlayerPlayAction,
	 EnemyPlayAction,
	         Waiting,
	       EndBattle,
	       NotBattle
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChangedSignature, EBattleState, NewState);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UBattleFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleFSMComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void ChangeState(EBattleState newState);
	EBattleState GetCurrentState();
	EBattleState GetBeforeState();

	FOnStateChangedSignature OnStateChanged;
	
protected:
	EBattleState CurrentState = EBattleState::NotBattle;  
	EBattleState BeforeState  = EBattleState::NotBattle;
	
	void      StartBattleState();
	void     SelectActionState();
	void      SelectSkillState();
	void     SelectTargetState();
	void PlayerPlayActionState();
	void  EnemyPlayActionState();
	void          WaitingState();
	void        EndBattleState();
	void        NotBattleState();
};
