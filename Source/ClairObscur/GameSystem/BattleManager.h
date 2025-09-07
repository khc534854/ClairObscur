// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"



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
	
	virtual void EnableInput(APlayerController* PlayerController) override;

	void SetParticipant();

	void BindInputActions();

	UFUNCTION()
	void OnFSMStateChanged(enum EBattleState NewState);

	UFUNCTION()
	void OnCharacterActionFinished();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Component
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleTimingComponent* BattleTimingComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleTurnComponent* BattleTurnComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleFSMComponent* BattleFSMComp;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	class UBattleUIComponent* BattleUIComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	TArray<ACharacter*> BattleParticipant;

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

	int32 SelectedSkillIndex;
	
};
