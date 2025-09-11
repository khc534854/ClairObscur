// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterComponent/SkillRow.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyMovement, bool, bIsMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParryWindowChanged, AEnemy*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyHPChangedSignature, float, CurrentHP, float, MaxHP, ACharacter*, DamagedActor);

struct FSkillRow;
class UEnemyAnimInstance;


UCLASS()
class CLAIROBSCUR_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* enemySkeletalMesh;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEnemyFSM* fsm;

	UFUNCTION()
	void EnemyIdle();
	UFUNCTION()
	void EnemyMove(FVector destination);
	UFUNCTION()
	void EnemyAttack();
	UFUNCTION()
	void EnemyDamage();
	UFUNCTION()
	void EnemyDie();

	UFUNCTION()
	void EnemySkill(const FVector& TargetLocation, int32 SkillIndex);

	const FSkillRow* GetSkillRowByIndex(int32 Index) const;

	UFUNCTION()
	void DestroySelf();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* attackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* damageAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* dieAnim;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnEnemyMovement OnEnemyMovement;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	TSoftObjectPtr<UDataTable> SkillTable;



	float currentTime;

	/*UFUNCTION()
	void OnEnemyNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void APlayerBase(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);*/

	UPROPERTY(VisibleAnywhere, Category="Combat")
	bool bCanBeParried = false;

	UPROPERTY(Transient)
	UEnemyAnimInstance* AnimInst = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnParryWindowChanged OnParryStart;

	UPROPERTY(BlueprintAssignable)
	FOnParryWindowChanged OnParryEnd;
	
	void StartCanParry();
	void EndCanParry();
//HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float maxHP = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float currentHP;

	void setEnemyHP(float hitdamage);
	float getEnemyHP();

	UPROPERTY(BlueprintAssignable)
	FOnEnemyHPChangedSignature OnHPChanged;
};
