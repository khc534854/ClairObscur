// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerDirectory/PlayerFSM.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

UPlayerAnimInstance();
	
public:
	UPROPERTY(BlueprintReadWrite, Category="FSM")
	ECommandedPlayerState AnimFsmState = ECommandedPlayerState::CombatIdle;

	UPROPERTY(BlueprintReadOnly, Category="FSM")
	bool    bMoveOut = false;      

	UPROPERTY(BlueprintReadOnly, Category="FSM")
	bool    bReturn  = false; 
	
	TWeakObjectPtr<class APlayerBase> OwnerPlayer;


	// notify
	
	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	
	// 나이아가라 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_OverChargeSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_OverChargeComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_SwordAttackSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_SwordAttackComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_GustavCounterAttackSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_GustavCounterAttackComp;

	
	// 루네 스킬
	// 1. 베이직 스킬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_LuneBasicAttackSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_LuneBasicAttackComp;

	// 2. 루네 제물 스킬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_LuneOneAttackSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_LuneOneAttackComp;
	

	// 3. 루네 얼음창 스킬 (2연타)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_LuneIceAttackSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_LuneIceAttackComp;

	// 4. 루네 카운터 스킬 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraSystem* NS_LuneCounterAttackSystem;     // 타격 순간 한번 터지는 FX

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	class UNiagaraComponent* NS_LuneCounterAttackComp;
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	



	
};
