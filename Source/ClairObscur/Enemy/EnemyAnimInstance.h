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
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category="Enemy")
	class AEnemy* ownerEnemy;

	/*
	UPROPERTY(BlueprintReadOnly, Category="Enemy")
	float speed;

	UPROPERTY(BlueprintReadOnly, Category="Enemy")
	bool bIsAttacking;

	UPROPERTY(BlueprintReadOnly, Category="Enemy")
	bool bIsDead;*/
};