// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	class AEnemy* OwnerEnemy = nullptr;
	
	// Called when an AnimNotify (or AnimNotifyState begin) fires
	void OnNotifyBegin(UAnimNotifyState* NotifyState);

	// AnimNotifyState 종료
	void OnNotifyEnd(UAnimNotifyState* NotifyState);

	
};
