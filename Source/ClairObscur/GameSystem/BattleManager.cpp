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
#include "Component/BattleCameraComponent.h"
#include "CharacterComponent/SkillComponent.h"

// Sets default values
ABattleManager::ABattleManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	
	BattleTimingComp = CreateDefaultSubobject<UBattleTimingComponent>(TEXT("BattleTimingComp"));
	BattleTurnComp = CreateDefaultSubobject<UBattleTurnComponent>(TEXT("BattleTurnComp"));
	BattleFSMComp = CreateDefaultSubobject<UBattleFSMComponent>(TEXT("BattleFSMComp"));
	BattleUIComp = CreateDefaultSubobject<UBattleUIComponent>(TEXT("BattleUIComp"));
	BattleCameraComp = CreateDefaultSubobject<UBattleCameraComponent>(TEXT("BattleCameraComp"));
	BattleCameraComp->SetupAttachment(DefaultSceneRoot);
	
	//생성자에서 인풋 넣어두자
	//ConstructorHelpers::FObjectFinder<UInputMappingContext>(TEXT(""));
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	BattleFSMComp->OnStateChanged.AddDynamic(this, &ABattleManager::OnFSMStateChanged);
	BattleFSMComp->OnStateChanged.AddDynamic(BattleUIComp, &UBattleUIComponent::OnFSMStateChanged);
}

void ABattleManager::StartBattle()
{
	SetParticipant();
	EnableInput(GetWorld()->GetFirstPlayerController()); // 인풋 주체 변경
	BindInputActions();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(this, 1.0f);
	}

	// 전투 시작 상태로 변경
	//BattleFSMComp->ChangeState(EBattleState::StartBattle);
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

	for (ACharacter* Character : BattleParticipant)
	{
		if (Character && Character->ActorHasTag(FName("Enemy")))
		{
			// "Enemy" 태그가 있으면 적군 목록에 추가합니다.
			EnemyParty.Add(Character);
		}
		else if(Character)
		{
			// 없으면 아군 목록에 추가합니다.
			PlayerParty.Add(Character);
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

void ABattleManager::OnCharacterActionFinished()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Action Finished. Advancing to next turn."));

	ACharacter* Character = BattleTurnComp->GetCurrentTurnCharacter();
	if (Character)
	{
		USkillComponent* SkillComp = Character->FindComponentByClass<USkillComponent>();
		if (SkillComp)
		{
			SkillComp->OnActionFinished.RemoveDynamic(this, &ABattleManager::OnCharacterActionFinished);
		}
	}

	BattleTurnComp->AdvanceTurn();
}

void ABattleManager::OnFSMStateChanged(EBattleState NewState)
{
	auto currentCharacter = BattleTurnComp->GetCurrentTurnCharacter();
	
	switch (NewState)
	{
	case EBattleState::StartBattle:
		break;
	case EBattleState::SelectAction:
		{
			FVector CamLocation = FVector(currentCharacter->GetActorLocation()) - FVector(150, -200, -20); // 목표 위치 계산
			FRotator CamRotation = FRotator(0, -30, -5); // 목표 회전 계산
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(FVector(currentCharacter->GetActorLocation()) - FVector(150, -200, -20), FRotator(0, -30, -5));
			break;
		}
	case EBattleState::SelectSkill:
		{
			FVector CamLocation = FVector(currentCharacter->GetActorLocation()) - FVector(110, -80, -80); // 목표 위치 계산
			FRotator CamRotation = FRotator(-15, -10, 5);
			BattleCameraComp->StartMoveWithCurve(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(FVector(currentCharacter->GetActorLocation()) - FVector(110, -80, -80), FRotator(-15, -10, 5));
			break;
		}
	case EBattleState::SelectTarget:
		{
			FVector CamLocation = EnemyParty[0]->GetActorLocation() - FVector(150,0, -50);
			FRotator CamRotation = FRotator(0); 
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(EnemyParty[0]->GetActorLocation() - FVector(150,0, -50), FRotator(0));
			break;
		}
	case EBattleState::PlayerPlayAction:
		{
			FVector CamLocation = FVector(-170, 340, 150);
			FRotator CamRotation = FRotator(0, -40, 0); 
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(FVector(-170, 340, 150), FRotator(0, -40, 0));
			break;
		}
	case EBattleState::EnemyPlayAction:
	{
		auto enemy = BattleTurnComp->GetCurrentTurnCharacter();
		if (enemy)
		{
			USkillComponent* skillComp = enemy->FindComponentByClass<USkillComponent>();
			if (skillComp)
			{
				skillComp->OnActionFinished.AddDynamic(this, &ABattleManager::OnCharacterActionFinished);
				skillComp->ExecuteSkill(FMath::RandRange(0, 3));
			}
		}
			//카메라 어떻게
		break;
	}
	case EBattleState::Waiting:
		break;
	case EBattleState::EndBattle:
		break;
	case EBattleState::NotBattle:
		break;
	default:
		break;
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
		SelectedSkillIndex = 1;
		return;
	}
	
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::EnemyPlayAction)
	{
		// Dodge
		return;
	}
}

void ABattleManager::WInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Skill 2
		// SelectedSkill = Player.SkillList[2];  
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);
		SelectedSkillIndex = 2;
		return;
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectAction)
	{
		// Item NotYet
		return;
	}
}

void ABattleManager::EInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Skill 3
		// SelectedSkill = Player.SkillList[3];   
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);
		SelectedSkillIndex = 3;
		return;
	}
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::EnemyPlayAction)
	{
		// parry
		return;
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectAction)
	{
		// Skill
		BattleFSMComp->ChangeState(EBattleState::SelectSkill);
		return;
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::PlayerPlayAction)
	{
		// Timing
		return;
	}
}

void ABattleManager::RInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		// Next Page Skill, Not Yet
		return;
	}
}

void ABattleManager::FInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectAction)
	{
		// Attack
		// SelectedSkill = Player.SkillList[0];  
		BattleFSMComp->ChangeState(EBattleState::SelectTarget);
		SelectedSkillIndex = 0;
		return;
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectTarget)
	{
		// select target
		BattleFSMComp->ChangeState(EBattleState::PlayerPlayAction);

		ACharacter* CurrentCharacter = BattleTurnComp->GetCurrentTurnCharacter();
		USkillComponent* SkillComp = CurrentCharacter->FindComponentByClass<USkillComponent>();
		if (SkillComp)
		{
			// 스킬 실행 전, "액션 끝나면 보고해!" 라고 구독을 신청합니다.
			SkillComp->OnActionFinished.AddDynamic(this, &ABattleManager::OnCharacterActionFinished);
			SkillComp->ExecuteSkill(SelectedSkillIndex);
		}
		return;
	}
}

void ABattleManager::ESCInputAction(const  FInputActionValue& Value)
{
	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectTarget)
	{
		BattleFSMComp->ChangeState(BattleFSMComp->GetBeforeState());
		return;
	}

	if (BattleFSMComp->GetCurrentState() == EBattleState::SelectSkill)
	{
		BattleFSMComp->ChangeState(EBattleState::SelectAction);
		// back
		return;
	}
}

