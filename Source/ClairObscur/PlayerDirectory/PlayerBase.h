// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

class UPlayerGrappleComponent;
struct FInputActionInstance;
struct FFindFloorResult;

class ADamageNumberActor;
class AWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackHitSignature, APlayerBase*, Attacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerHPChangedSignature, float, CurrentHP, float, MaxHP, ACharacter*, DamagedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerAPChangedSignature, int32, CurrentAP, ACharacter*, UseCostActor);

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

	// skill component 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkillComponent* skillComp;

	// damage component 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UDamagePopupComponent* damageComp;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UPlayerGrappleComponent* grappleComp = nullptr;



public: // input

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputMappingContext* IMC_Player;
	
	// 이동
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_move;
	FVector direction = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_run;

	// 이동처리
	void RunInput(const struct FInputActionValue& value);
	void MoveInput(const struct FInputActionValue& value);

	UPROPERTY(EditAnywhere, Category = "Settings")
	float walkSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float runSpeed = 600.f;

	// 회전
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_lookup;

	// 회전 처리
	void TurnInput(const struct FInputActionValue& value);
	void LookupInput(const struct FInputActionValue& value);


	// 점프
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_jump;

	void JumpInput(const struct FInputActionValue& value);


	
public: // 무기 
	// 무기 소환
	UFUNCTION(BlueprintCallable)
	void SpawnWeapon();

	UFUNCTION(BlueprintCallable)
	void DestroyWeapon();


	// 무기 클래스, 쿠기 인스턴스
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	UPROPERTY(Transient)
	AWeaponBase* WeaponInstance;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName SwordSocketName = TEXT("RightHandSocket");

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	bool bHasWeapon = false;
	
	UFUNCTION()
	void OnToggleWeapon_Triggered(const FInputActionInstance& Instance);


public: // 스탯
	UPROPERTY(EditAnywhere, Category = "Stat")
	int32 maxHP = 150;
	
	UPROPERTY(EditAnywhere, Category = "Stat")
	int32 currentHP;

	UPROPERTY(EditAnywhere, Category = "Stat")
	int32 maxAP = 9;

	UPROPERTY(EditAnywhere, Category = "Stat")
	int32 currentAP;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerHPChangedSignature OnHPChanged;
	
	//체력, AP function get / set
	UFUNCTION()
	void setplayerHP(int32 hitdamage, AActor* DamageCauser);
	
	UFUNCTION()
	int32 getplayerHP() const;
	
	UFUNCTION()
    void setplayerAP(int32 useAP);
    	
    UFUNCTION()
    int32 getplayerAP() const; 


public:
	// AnimNotify가 호출할 함수
	void OnAttackHit();


	// BattleManager가 구독할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnAttackHitSignature OnAttackHitDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerAPChangedSignature OnUseAPDelegate;


	// damage ui
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
				 AController* EventInstigator, AActor* DamageCauser);



	

 
};

