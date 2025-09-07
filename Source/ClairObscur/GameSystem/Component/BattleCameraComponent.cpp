// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "BattleFSMComponent.h"
#include "BattleTurnComponent.h"
#include "ClairObscur/GameSystem/BattleManager.h" 


// Sets default values for this component's properties
UBattleCameraComponent::UBattleCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(this);
}


// Called when the game starts
void UBattleCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBattleCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector NewLocation = FMath::VInterpTo(MainCamera->GetComponentLocation(), TargetLocation, DeltaTime, CameraInterpSpeed);
	MainCamera->SetWorldLocation(NewLocation);

	FRotator NewRotation = FMath::RInterpTo(MainCamera->GetComponentRotation(), TargetRotation, DeltaTime, CameraInterpSpeed);
	MainCamera->SetWorldRotation(NewRotation);
}

void UBattleCameraComponent::OnFSMStateChanged(EBattleState NewState)
{
	auto* OwnerManager = GetOwner<ABattleManager>();
	if (!OwnerManager || !OwnerManager->BattleTurnComp) return;

	ACharacter* CurrentCharacter = OwnerManager->BattleTurnComp->GetCurrentTurnCharacter();
	if (!CurrentCharacter) return;

	switch (NewState)
	{
	case EBattleState::SelectAction:
		break;
	case EBattleState::SelectSkill:
		break;
	case EBattleState::SelectTarget:
		break;
	case EBattleState::PlayerPlayAction:
		break;
	case EBattleState::EnemyPlayAction:
		break;
	case EBattleState::StartBattle:
		break;
	case EBattleState::Waiting:
		break;
	case EBattleState::EndBattle:
		break;
	case EBattleState::NotBattle:
		break;
	default:;
	}
}

