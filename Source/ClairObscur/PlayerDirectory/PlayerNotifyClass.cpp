// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerNotifyClass.h"
#include "ClairObscur/PlayerDirectory/PlayerBase.h"

void UPlayerNotifyClass::Notify(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	// 이 애니메이션을 재생하고 있는 액터를 가져옵니다.
	APlayerBase* PlayerCharacter = Cast<APlayerBase>(MeshComp->GetOwner());
	if (PlayerCharacter)
	{
		// 플레이어 캐릭터에게 "지금 공격이 적중했다!"고 알려줍니다.
		PlayerCharacter->OnAttackHit();
	}
	
}
