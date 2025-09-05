// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"

#include "Component/BattleTimingComponent.h"
#include "Component/BattleTurnComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/BattleFSMComponent.h"
#include "Component/BattleUIComponent.h"


// Sets default values
ABattleManager::ABattleManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BattleTimingComp = CreateDefaultSubobject<UBattleTimingComponent>(TEXT("BattleTimingComp"));
	BattleTurnComp = CreateDefaultSubobject<UBattleTurnComponent>(TEXT("BattleTurnComp"));
	BattleFSMComp = CreateDefaultSubobject<UBattleFSMComponent>(TEXT("BattleFSMComp"));
	BattleUIComp = CreateDefaultSubobject<UBattleUIComponent>(TEXT("BattleUIComp"));
	//생성자에서 인풋 넣어두자
	//ConstructorHelpers::FObjectFinder<UInputMappingContext>(TEXT(""));
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleManager::StartBattle()
{
	SetParticipant();
	BindInputActions();
	EnableInput(GetWorld()->GetFirstPlayerController()); // 인풋 주체 변경
}

void ABattleManager::EnableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);
	
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IMC_BattleManager)
			{
				Subsystem->AddMappingContext(IMC_BattleManager, 0);
			}
		}
	}
}


// 전투 참여자 등록
void ABattleManager::SetParticipant()
{
	BattleParticipant.Empty();

	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BattlePossible"), ActorsWithTag);

	
	for (AActor* Actor : ActorsWithTag)
	{
		ACharacter* Participant = Cast<ACharacter>(Actor);
		if (Participant)
		{
			BattleParticipant.Add(Participant);
		}
	}
	BattleTurnComp->StartBattle();
}

void ABattleManager::BindInputActions()
{
	if (InputComponent)
	{
		if (UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(InputComponent))
		{
			input->BindAction(IA_Q,   ETriggerEvent::Started, this, &ABattleManager::QInputAction);
			input->BindAction(IA_W,   ETriggerEvent::Started, this, &ABattleManager::WInputAction);
			input->BindAction(IA_E,   ETriggerEvent::Started, this, &ABattleManager::EInputAction);
			input->BindAction(IA_R,   ETriggerEvent::Started, this, &ABattleManager::RInputAction);
			input->BindAction(IA_F,   ETriggerEvent::Started, this, &ABattleManager::FInputAction);
			input->BindAction(IA_ESC, ETriggerEvent::Started, this, &ABattleManager::ESCInputAction);
		}
	}
}


// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABattleManager::QInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Skill 1
		// SelectedSkill = Player.SkillList[1];  
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);
	}
	
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::EnemyPlayAction)
	{
		// Dodge
	}
}

void ABattleManager::WInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Skill 2
		// SelectedSkill = Player.SkillList[2];  
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);

	}
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectAction)
	{
		// Item NotYet
	}
}

void ABattleManager::EInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Skill 3
		// SelectedSkill = Player.SkillList[3];   
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);

	}
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::EnemyPlayAction)
	{
		// parry
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectAction)
	{
		// Skill
		BattleFSMComp->ChangeState(EBattleState::SelectSkill);
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::PlayerPlayAction)
	{
		// Timing
	}
}

void ABattleManager::RInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Next Page Skill, Not Yet
	}
}

void ABattleManager::FInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectAction)
	{
		// Attack
		// SelectedSkill = Player.SkillList[0];  
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectTarget)
	{
		// select target
		BattleFSMComp->ChangeState(EBattleState::PlayerPlayAction);
	}
}

void ABattleManager::ESCInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectTarget)
	{
		BattleFSMComp->ChangeState(BattleFSMComp->GetBeforeState());
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		BattleFSMComp->ChangeState(EBattleState::SelectAction);
		// back
	}
}

