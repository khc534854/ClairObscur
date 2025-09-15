// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle    UMETA(DisplayName="Idle"),
	Move    UMETA(DisplayName="Move"),
	Attack  UMETA(DisplayName="Attack"),
	Damage  UMETA(DisplayName="Damage"),
	Die     UMETA(DisplayName="Die")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:


	// 컴포넌트 소유자 선언
	UPROPERTY()
	class AEnemy* me;



	void IdleState();

	// 필요속성 : 타겟
	UPROPERTY()
	class APlayerBase* target;

	void MoveState();
	void AttackState();

	// 피격 대기 시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float damageDelayTime = 2.0f;
		
	void DamageState();
	void DieState();

	// 피격시 호출될 이벤트 함수 -> 콜백
	UPROPERTY(EditDefaultsOnly, Category=FSM)
	int32 MAX_HP = 3;
	int32 hp = MAX_HP;

	// 필요속성 : 넉백 파워
	UPROPERTY(EditAnywhere, Category=FSM)
	float knockbackPower = 10;
	FVector knockbackPos;
	void OnDamageProcess(FVector hitDirection);

	
	void SetTargetToMove(FVector targetVect);

	float delayTime;
	float currentTime;

	UFUNCTION(BlueprintCallable)
	void SetEnemyState(EEnemyState NewState);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FSM)
	EEnemyState CurrentState = EEnemyState::Idle;


	UFUNCTION()
	void OnParryWindowOpened();

	UFUNCTION()
	void OnParryWindowClosed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Parry")
	bool bCanBeParried = false;

	UFUNCTION()
	EEnemyState GetState();

	UPROPERTY()
	FVector targetVector = FVector(-50.000000,220.000000,50.000000);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool didIAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector moveDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float moveSpeed = 300;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool inAttackRange = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector enemyOriginLocation = FVector(660.000000,240.000000,88.000000);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector returnDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float distanceTarget = 200;

	



};
