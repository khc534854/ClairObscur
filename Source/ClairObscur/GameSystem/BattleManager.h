// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"


enum class ETimingMode : uint8;

UCLASS()
class CLAIROBSCUR_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABattleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartBattle();

	UFUNCTION(BlueprintCallable)
	void EndBattle();
	
	virtual void EnableInput(APlayerController* PlayerController) override;
	virtual void DisableInput(APlayerController* PlayerController) override;

	void SetParticipant();

	void BindInputActions();

	UFUNCTION()
	void OnFSMStateChanged(enum EBattleState NewState);

	UFUNCTION()
	void OnPlayerActionFinished(int SkillIndex,  bool bInterrupted, bool bReachedSpot);

	UFUNCTION()
	void OnEnemyActionFinished();

	UFUNCTION()
	void OnCharacterHPChanged(float CurrentHP, float MaxHP, ACharacter* DamagedActor);

	UFUNCTION()
	void OnPlayerAPChanged(int32 CurrentAP);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Component
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// USceneComponent* DefaultSceneRoot;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleTimingComponent* BattleTimingComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleTurnComponent* BattleTurnComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleFSMComponent* BattleFSMComp;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleUIComponent* BattleUIComp;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleCameraComponent* BattleCameraComp;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleDamageCalculateComponent* BattleDamageCalcComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleFieldComponent* BattleFieldComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	TArray<ACharacter*> BattleParticipant;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	TArray<ACharacter*> PlayerParty; // 아군 목록

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	TArray<ACharacter*> EnemyParty; // 적군 목록

	UPROPERTY()
	class AEnemy* CurrentTargetEnemy;

	UPROPERTY()
	class APlayerBase* CurrentTargetPlayer;

public:
	// Input
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* IMC_BattleManager;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Q;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_W;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_E;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_R;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_F;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_ESC;

	void   QInputAction(const struct FInputActionValue& Value);
	void   WInputAction(const struct FInputActionValue& Value);
	void   EInputAction(const struct FInputActionValue& Value);
	void   RInputAction(const struct FInputActionValue& Value);
	void   FInputAction(const struct FInputActionValue& Value);
	void ESCInputAction(const struct FInputActionValue& Value);
	UFUNCTION()
	void OnTimingCheckResult(bool bSuccess, ETimingMode TimingMode);

	int32 SelectedSkillIndex;
	bool pressedQTE = false;
	
public:
	// Notify
	UFUNCTION()
	void HandleParryStart(class AEnemy* Enemy);

	UFUNCTION()
	void HandleParryEnd(class AEnemy* Enemy);

	UFUNCTION()
	void HandlePlayerAttackHit(APlayerBase* Attacker);

	UFUNCTION()
	void HandleEnemyAttackHit(AEnemy* Attacker);

};
