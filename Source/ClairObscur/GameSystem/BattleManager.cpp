// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"

#include "Component/BattleTimingComponent.h"
#include "Component/BattleTurnComponent.h"
#include "Component/BattleFSMComponent.h"
#include "Component/BattleUIComponent.h"
#include "Component/BattleCameraComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterComponent/SkillComponent.h"
#include "PlayerDirectory/PlayerBase.h"
#include "PlayerDirectory/PlayerFSM.h"
#include "Widget/SelectSkillWidget.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyFSM.h"

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
	// 인풋 주체 변경
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		auto PlayerPawn = Cast<APlayerBase>(PlayerController->GetPawn());
		if (PlayerPawn) // Pawn이 유효했을 경우에만 DisableInput 호출
		{
			PlayerPawn->DisableInput(PlayerController);
		}
   
		PlayerController->SetViewTargetWithBlend(this, 1.0f);
	}
	
	SetParticipant();
	EnableInput(GetWorld()->GetFirstPlayerController());
	BindInputActions();
	
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
				Subsystem->AddMappingContext(IMC_BattleManager, 1);
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
			EnemyParty.Add(Character);
		}
		else if(Character && Character->ActorHasTag(FName("Player")))
		{
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

// void ABattleManager::OnCharacterActionFinished(int SkillIndex,  bool bInterrupted, bool bReachedSpot)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Action Finished. Advancing to next turn."));
//
// 	ACharacter* Character = BattleTurnComp->GetCurrentTurnCharacter();
// 	
// 	if (Character->ActorHasTag(FName("Enemy")))
// 	{
// 		USkillComponent* SkillComp = Character->FindComponentByClass<USkillComponent>();
// 		if (SkillComp)
// 		{
// 			SkillComp->OnActionFinished.RemoveDynamic(this, &ABattleManager::OnCharacterActionFinished);
// 		}
// 	}
// 	else if (Character->ActorHasTag(FName("Player")))
// 	{
// 		Cast<APlayerBase>(Character)->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnCharacterActionFinished);
// 		
// 	}
//
// 	BattleTurnComp->AdvanceTurn();
// }
//
// void ABattleManager::OnCharacterActionFinished()
// {
// }

void ABattleManager::OnFSMStateChanged(EBattleState NewState)
{
	auto currentCharacter = BattleTurnComp->GetCurrentTurnCharacter();
	pressedQTE = false;
	
	auto player = Cast<APlayerBase>(currentCharacter);
	switch (NewState)
	{
	case EBattleState::StartBattle:
		break;
	case EBattleState::SelectAction:
		{
			player->fsm->SetCommandedState(ECommandedPlayerState::SelectAction);
			FVector CamLocation = FVector(currentCharacter->GetActorLocation()) - FVector(150, -200, -20); // 목표 위치 계산
			FRotator CamRotation = FRotator(0, -30, -5); // 목표 회전 계산
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(FVector(currentCharacter->GetActorLocation()) - FVector(150, -200, -20), FRotator(0, -30, -5));
			break;
		}
	case EBattleState::SelectSkill:
		{
			player->fsm->SetCommandedState(ECommandedPlayerState::SelectSkill);
			FVector CamLocation = FVector(currentCharacter->GetActorLocation()) - FVector(110, -80, -80); // 목표 위치 계산
			FRotator CamRotation = FRotator(-15, -10, 5);
			BattleCameraComp->StartMoveWithCurve(CamLocation, CamRotation, 5.0f);

			UDataTable* SkillTable = Cast<APlayerBase>(currentCharacter)->fsm->SkillTable.LoadSynchronous();
			if (SkillTable)
			{
				TArray<FSkillRow*> SkillRows;
				SkillTable->GetAllRows(TEXT(""), SkillRows);
				Cast<USelectSkillWidget>(BattleUIComp->SelectSkillWidget)->PopulateSkills(SkillRows);
			}
			
			//Cast<USelectSkillWidget>(BattleUIComp->SelectSkillWidget)->PopulateSkills(Cast<APlayerBase>(currentCharacter)->fsm->SkillTable);
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
			player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
			BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);

			FVector CamLocation = FVector(-170, 340, 150);
			FRotator CamRotation = FRotator(0, -40, 0); 
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(FVector(-170, 340, 150), FRotator(0, -40, 0));
			break;
		}
	case EBattleState::EnemyPlayAction:
	{
		AEnemy* CurrentEnemy = Cast<AEnemy>(currentCharacter);
		if (CurrentEnemy)
		{
			// 1. 적 FSM에게 공격 상태로 전환하라고 직접 명령합니다.
			//    (EnemyFSM의 Tick 로직 대신 BattleManager가 흐름을 제어)
			if (CurrentEnemy->fsm)
			{
				CurrentEnemy->fsm->SetEnemyState(EEnemyState::Attack);
			}

			// 2. 공격 애니메이션 길이를 가져옵니다.
			float AttackAnimLength = 0.f;
			if (CurrentEnemy->attackAnim) // AEnemy에 기본 공격 몽타주가 할당되어 있다고 가정
			{
				AttackAnimLength = CurrentEnemy->attackAnim->GetPlayLength();
			}

			// 3. 애니메이션 길이가 끝나면 턴을 넘기도록 타이머를 설정합니다.
			FTimerHandle EnemyTurnTimer;
			GetWorld()->GetTimerManager().SetTimer(
				EnemyTurnTimer,
				this,
				&ABattleManager::OnEnemyActionFinished, // 턴 넘기는 함수
				AttackAnimLength, // 애니메이션 길이만큼 기다림
				false);
		}
		break;

		//auto enemy = BattleTurnComp->GetCurrentTurnCharacter();
		//if (enemy)
		//{
		//	USkillComponent* skillComp = enemy->FindComponentByClass<USkillComponent>();
		//	if (skillComp)
		//	{
		//		skillComp->OnActionFinished.AddDynamic(this, &ABattleManager::OnEnemyActionFinished);
		//		skillComp->ExecuteSkill(FMath::RandRange(0, 3));
		//	}
		//}
		//	//카메라 어떻게
		//	// 회피, 피격, 패링 처리
		//break;
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

void ABattleManager::OnPlayerActionFinished(int SkillIndex, bool bInterrupted, bool bReachedSpot)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Action Finished. Advancing to next turn."));
	ACharacter* Character = BattleTurnComp->GetCurrentTurnCharacter();
	Cast<APlayerBase>(Character)->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
	BattleTurnComp->AdvanceTurn();
}

void ABattleManager::OnEnemyActionFinished()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Action Finished. Advancing to next turn."));

	//ACharacter* Character = BattleTurnComp->GetCurrentTurnCharacter();
	//
	//if (Character->ActorHasTag(FName("Enemy")))
	//{
	//	USkillComponent* SkillComp = Character->FindComponentByClass<USkillComponent>();
	//	if (SkillComp)
	//	{
	//		SkillComp->OnActionFinished.RemoveDynamic(this, &ABattleManager::OnEnemyActionFinished);
	//	}
	//}
	BattleTurnComp->AdvanceTurn();
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
		BattleTimingComp->OnPlayerInput();
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

		auto CurrentCharacter = Cast<APlayerBase>(BattleTurnComp->GetCurrentTurnCharacter());
		if (CurrentCharacter && CurrentCharacter->fsm)
		{
			CurrentCharacter->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);

			// TODO: 현재 타겟팅된 적을 가져오는 로직 필요
			AEnemy* TargetEnemy = Cast<AEnemy>(EnemyParty[0]); // 임시로 첫 번째 적을 타겟으로 지정

			if (TargetEnemy)
			{
				CurrentCharacter->fsm->ExecuteSkill(TargetEnemy->GetActorLocation(), SelectedSkillIndex);

				// 3. (임시) 스킬 사용 즉시 타겟에게 데미지를 입히라고 명령
				// 원래는 플레이어의 공격 애니메이션 중간에 호출되어야 합니다.
				TargetEnemy->EnemyDamage();
			}
		}
		//auto CurrentCharacter = Cast<APlayerBase>(BattleTurnComp->GetCurrentTurnCharacter());
		//CurrentCharacter->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
		//CurrentCharacter->fsm->ExecuteSkill(EnemyParty[0]->GetActorLocation(), SelectedSkillIndex);
		//return;
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

void ABattleManager::OnTimingCheckResult(bool bSuccess)
{
	if (bSuccess)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Timing Success"));
		// TODO: 데미지 증가 등 성공 처리
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timing Fail"));
		// TODO: 실패 처리
	}
}

