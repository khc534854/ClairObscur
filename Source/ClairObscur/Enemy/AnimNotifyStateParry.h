// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyStateParry.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UAnimNotifyStateParry : public UAnimNotifyState
{
	GENERATED_BODY()

	
public:
	// Notify 시작 (Deprecated 없는 시그니처 사용)
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	// Notify 종료
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};