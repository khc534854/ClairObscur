// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleUIComponent.h"

#include "BattleFSMComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameSystem/BattleManager.h"
#include "GameSystem/Widget/BattleHUDWidget.h"
#include "GameSystem/Widget/SelectSkillWidget.h"
#include "GameSystem/Widget/WidgetComponent/QTEWidget.h"
#include "GameSystem/Widget/WidgetComponent/SkillDetailWidget.h"


class ABattleManager;
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

	auto PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (SelectActionWidgetClass)
	{
		SelectActionWidget = CreateWidget<UUserWidget>(PC, SelectActionWidgetClass);
		AllWidgets.Add(SelectActionWidget);
	}
	if (SelectSkillWidgetClass)
	{
		SelectSkillWidget = CreateWidget<UUserWidget>(PC, SelectSkillWidgetClass);
		AllWidgets.Add(SelectSkillWidget);
	}
	if (TimingWidgetClass)
	{
		TimingWidget = CreateWidget<UUserWidget>(PC, TimingWidgetClass);
		AllWidgets.Add(TimingWidget);
	}
	if (SelectTargetWidgetClass)
	{
		SelectTargetWidget = CreateWidget<UUserWidget>(PC, SelectTargetWidgetClass);
		AllWidgets.Add(SelectTargetWidget);
	}
	if (PlayerHUDWidgetClass)
	{
		BattleHUDWidget = CreateWidget<UBattleHUDWidget>(PC, PlayerHUDWidgetClass);
		BattleHUDWidget->AddToViewport();
	}

	HideAllWidgets();
}

void UBattleUIComponent::HideAllWidgets()
{
	for (UUserWidget* Widget : AllWidgets)
	{
		if (Widget && Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}
	}
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
	HideAllWidgets();
	
	switch (NewState)
	{
	case EBattleState::SelectAction:
		if (SelectActionWidget)
		{
			SelectActionWidget->AddToViewport();
		}
		break;
	case EBattleState::SelectSkill:
		if (SelectSkillWidget)
		{
			auto ui = Cast<USelectSkillWidget>(SelectSkillWidget);
			SelectSkillWidget->AddToViewport();
		}
		break;
	case EBattleState::SelectTarget:
		if (SelectTargetWidget)
		{
			SelectTargetWidget->AddToViewport();
		}
		break;
	case EBattleState::PlayerPlayAction:
		if (TimingWidget)
		{
			ShowQTEWidget();
			TimingWidget->AddToViewport();
		}
		break;
	case EBattleState::EnemyPlayAction:
		if (TimingWidget)
		{
			//ShowQTEWidget();
			//TimingWidget->AddToViewport();
		}
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
	}
}

void UBattleUIComponent::ShowQTEWidget()
{
	if (TimingWidget)
	{
		ABattleManager* OwnerManager = GetOwner<ABattleManager>();
		if (OwnerManager && OwnerManager->BattleTimingComp)
		{
			Cast<UQTEWidget>(TimingWidget)->SetOwnerTimingComponent(OwnerManager->BattleTimingComp);
		}
	}
}

void UBattleUIComponent::UpdateHUD()
{
}



