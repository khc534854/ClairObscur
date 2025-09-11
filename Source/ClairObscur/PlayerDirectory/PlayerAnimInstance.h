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
	
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	



	
};
