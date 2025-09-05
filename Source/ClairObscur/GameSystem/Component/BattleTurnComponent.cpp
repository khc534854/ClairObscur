// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnComponent.h"

#include "BattleFSMComponent.h"
#include "ClairObscur/CharacterComponent/SkillComponent.h"
#include "ClairObscur/GameSystem/BattleManager.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UBattleTurnComponent::UBattleTurnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleTurnComponent::BeginPlay()
{
	Super::BeginPlay();

	BM = Cast<ABattleManager>(GetOwner());
}

void UBattleTurnComponent::StartBattle()
{
	if (!BM) return;

	CurrentTurnIndex = -1;
	AdvanceTurn();
}

void UBattleTurnComponent::AdvanceTurn()
{
	if (!BM || BM->BattleParticipant.Num() == 0) return;

	CurrentTurnIndex = (CurrentTurnIndex + 1) % BM->BattleParticipant.Num();
    
	BeginNewTurn();
}

void UBattleTurnComponent::BeginNewTurn()
{
	ACharacter* CurrentCharacter = GetCurrentTurnCharacter();
    
	// BattleManager와 FSM컴포넌트, 현재 캐릭터가 모두 유효한지 확인
	if (!BM || !BM->BattleFSMComp || !CurrentCharacter) return;

	if (CurrentCharacter->ActorHasTag(FName("Enemy")))
	{
		// Enemy 태그가 있다면, FSM 상태를 EnemyPlayAction으로 변경합니다.
		BM->BattleFSMComp->ChangeState(EBattleState::EnemyPlayAction);
        
		FString msg = FString::Printf(TEXT("Turn Start: %s (Enemy)"), *CurrentCharacter->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, msg);

		auto skillData = Cast<USkillComponent>(CurrentTurnCharacter->GetComponentByClass(USkillComponent::StaticClass()))->SkillDataAssets;
		CurrentCharacter->PlayAnimMontage(skillData[0]->SkillMontage);
	}
	else
	{
		// Enemy 태그가 없다면 플레이어로 간주하고, FSM 상태를 SelectAction으로 변경합니다.
		BM->BattleFSMComp->ChangeState(EBattleState::SelectAction);

		FString msg = FString::Printf(TEXT("Turn Start: %s (Player)"), *CurrentCharacter->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, msg);
	}
}

ACharacter* UBattleTurnComponent::GetCurrentTurnCharacter() const
{
	if (BM && BM->BattleParticipant.IsValidIndex(CurrentTurnIndex))
	{
		return BM->BattleParticipant[CurrentTurnIndex];
	}
	return nullptr;
}



// Called every frame
void UBattleTurnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

