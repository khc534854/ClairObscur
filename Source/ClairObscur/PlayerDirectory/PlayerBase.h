// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

struct FInputActionInstance;
struct FFindFloorResult;

class AWeaponBase;


UCLASS()
class CLAIROBSCUR_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public: // components
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* cameraComp;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* springArmComp;

	// FSM 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayerFSM* fsm;

	// control모드
	bool IsFreeControl() const;

	void EnterCommandMode();
	void ExitCommandMode();
	

public: // input

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputMappingContext* IMC_Player;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_MoveForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_MoveRight;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_JogOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_LookTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_LookUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_CustomJump;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_SpwanWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* IA_AttackQ;
	
public: // input function

	UFUNCTION()
	FVector2D GetMoveSpeed() const;
	
	UFUNCTION()
	void MoveToFloor();
	
	UFUNCTION()
	void HandleLookUpInput(const struct FInputActionValue& value);
	
	UFUNCTION()
	void HandleRightInput(float value);
	
	UFUNCTION()
	void HandleForwardInput(float value);
	
	UFUNCTION()
	void HandleTurnInput(const struct FInputActionValue& value);

	UFUNCTION()
	void MoveForward_Triggered(const FInputActionInstance& Instance);
	

	UFUNCTION()
	void MoveRight_Triggered(const FInputActionInstance& Instance);


	UFUNCTION()
	void JogOverrideAction_Triggered(const FInputActionInstance& Instance);

	UFUNCTION()
	void JogOverrideAction_Finished(const FInputActionInstance& Instance);
	
	UFUNCTION()
	void PlayerJump();

	//공격
	UFUNCTION()
	void PlayerAttackQ();


public: // input variables
	UPROPERTY(EditDefaultsOnly)
	float IA_Move_Forward_Action_Value;

	UPROPERTY(EditDefaultsOnly)
	float IA_Move_Right_Action_Value;
	
	UPROPERTY(EditDefaultsOnly)
	float InitialStepHeight;
	
	UPROPERTY(EditDefaultsOnly)
	float LargeStepHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	class UAnimMontage* montage_q;
	
public:
	// 무기 소환
	UFUNCTION(BlueprintCallable)
	void SpawnWeapon();

	UFUNCTION(BlueprintCallable)
	void DestroyWeapon();


	// 검 클래스 & 인스턴스
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> SwordClass;

	UPROPERTY(Transient)
	AWeaponBase* SwordInstance;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName SwordSocketName = TEXT("RightHandSocket");

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	bool bHasWeapon = false;
	
	UFUNCTION()
	void OnToggleWeapon_Triggered(const FInputActionInstance& Instance);
	
};

