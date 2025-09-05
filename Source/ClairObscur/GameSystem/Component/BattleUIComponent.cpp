// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleUIComponent.h"

#include "BattleFSMComponent.h"
#include "Blueprint/UserWidget.h"


// Sets default values for this component's properties
UBattleUIComponent::UBattleUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleUIComponent::BeginPlay()
{
	Super::BeginPlay();

	//SelectActionWidget	 = CreateWidget(this, UUserWidget::StaticClass());
	//SelectSkillWidget	 = CreateWidget(this, UUserWidget::StaticClass());
	//TimingWidget		 = CreateWidget(this, UUserWidget::StaticClass());
	//SelectTargetWidget	 = CreateWidget(this, UUserWidget::StaticClass());
}


// Called every frame
void UBattleUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBattleUIComponent::OnFSMStateChanged(EBattleState NewState)
{
	switch (NewState)
	{
	case EBattleState::SelectAction:
		// 행동 선택(공격, 스킬, 아이템) UI를 화면에 표시하는 로직
				
		break;
    
	case EBattleState::SelectSkill:
		// 스킬 선택 UI를 화면에 표시하는 로직
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("UI: Show Skill Select Widget"));
		break;

	case EBattleState::SelectTarget:
		// 타겟 선택 UI를 화면에 표시하는 로직
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("UI: Show Target Select Widget"));
		break;

	case EBattleState::PlayerPlayAction:
	case EBattleState::EnemyPlayAction:
		// 모든 선택 UI를 숨기는 로직
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("UI: Hide All Select Widgets"));
		break;
	case EBattleState::StartBattle:
		break;
	case EBattleState::Waiting:
		break;
	case EBattleState::EndBattle:
		break;
	case EBattleState::NotBattle:
		break;
	default: ;

		// ... 기타 상태에 대한 처리
	}
}

