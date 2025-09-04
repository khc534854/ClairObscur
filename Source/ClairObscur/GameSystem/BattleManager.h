// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"

enum class BattleState
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

	void StartBattle();
	virtual void EnableInput(APlayerController* PlayerController) override;

	void SetParticipant();

	void BindInputActions();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* BattleMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* EndTurnAction;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class UBattleTimingComponent* BattleTimingComp;
	
	UPROPERTY()
	TArray<ACharacter*> BattleParticipant;

	UPROPERTY()
	class UBattleTurnComponent* BattleTurnComp;

	BattleState CurrentState = BattleState::NotBattle;
	BattleState BeforeState  = BattleState::NotBattle;

	// Input
public:
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

	void QInputAction(const class FInputActionValue&  Value);
	void WInputAction(const class FInputActionValue&  Value);
	void EInputAction(const class FInputActionValue&  Value);
	void RInputAction(const class FInputActionValue&  Value);
	void FInputAction(const class FInputActionValue&  Value);
	void ESCInputAction(const class FInputActionValue&  Value);
	
	
};
