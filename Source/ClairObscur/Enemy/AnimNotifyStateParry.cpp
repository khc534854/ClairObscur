// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyStateParry.h"
#include "Enemy.h"
#include "EnemyFSM.h"

void UAnimNotifyStateParry::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;

	AEnemy* ownerEnemy = Cast<AEnemy>(MeshComp->GetOwner());
	if (!ownerEnemy) return;

	// Parry 창 열기
	if (ownerEnemy->fsm)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotifyStateParry::NotifyBegin"));
		ownerEnemy->bCanBeParried = true;
		ownerEnemy->fsm->bCanBeParried = true;
		ownerEnemy->fsm->OnParryWindowOpened();
	}
}

void UAnimNotifyStateParry::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	AEnemy* ownerEnemy = Cast<AEnemy>(MeshComp->GetOwner());
	if (!ownerEnemy) return;

	// Parry 창 닫기
	if (ownerEnemy->fsm)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotifyStateParry::NotifyEnd"));

		ownerEnemy->bCanBeParried = false;
		ownerEnemy->fsm->bCanBeParried = false;
		ownerEnemy->fsm->OnParryWindowClosed();
	}
}
