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

public:
	UPROPERTY(BlueprintReadOnly, Category="FSM")
	ECommandedPlayerState AnimFsmState = ECommandedPlayerState::CombatIdle;
	
	TWeakObjectPtr<class APlayerBase> OwnerPlayer;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;



	
};
