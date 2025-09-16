// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"

#include "Component/BattleTimingComponent.h"
#include "Component/BattleTurnComponent.h"
#include "Component/BattleFSMComponent.h"
#include "Component/BattleUIComponent.h"
#include "Component/BattleCameraComponent.h"
#include "Component/BattleDamageCalculateComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Component/BattleFieldComponent.h"
#include "PlayerDirectory/PlayerBase.h"
#include "PlayerDirectory/PlayerFSM.h"
#include "Widget/SelectSkillWidget.h"
#include "Widget/BattleHUDWidget.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyFSM.h"

// Sets default values
ABattleManager::ABattleManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BattleTimingComp = CreateDefaultSubobject<UBattleTimingComponent>(TEXT("BattleTimingComp"));
	BattleTurnComp = CreateDefaultSubobject<UBattleTurnComponent>(TEXT("BattleTurnComp"));
	BattleFSMComp = CreateDefaultSubobject<UBattleFSMComponent>(TEXT("BattleFSMComp"));
	BattleUIComp = CreateDefaultSubobject<UBattleUIComponent>(TEXT("BattleUIComp"));
	BattleDamageCalcComp = CreateDefaultSubobject<UBattleDamageCalculateComponent>(TEXT("BattleDamageCalcComp"));
	BattleCameraComp = CreateDefaultSubobject<UBattleCameraComponent>(TEXT("BattleCameraComp"));
	BattleFieldComp = CreateDefaultSubobject<UBattleFieldComponent>(TEXT("BattleFieldComp"));
	
	RootComponent = BattleFieldComp;
	BattleFieldComp->BaseComp->SetupAttachment(BattleFieldComp);
	BattleFieldComp->PlayerPos1->SetupAttachment(BattleFieldComp->BaseComp);
	BattleFieldComp->PlayerPos2->SetupAttachment(BattleFieldComp->BaseComp);
	BattleFieldComp->EnemyPos->SetupAttachment(BattleFieldComp->BaseComp);
	
	BattleCameraComp->MainCamera->SetupAttachment(BattleCameraComp);
	BattleCameraComp->SetupAttachment(RootComponent);
	//생성자에서 인풋 넣어두자
	//ConstructorHelpers::FObjectFinder<UInputMappingContext>(TEXT(""));
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	BattleFSMComp->OnStateChanged.AddDynamic(this, &ABattleManager::OnFSMStateChanged);
	BattleFSMComp->OnStateChanged.AddDynamic(BattleUIComp, &UBattleUIComponent::OnFSMStateChanged);

	BattleTimingComp->OnTimingResult.AddDynamic(this, &ABattleManager::OnTimingCheckResult);
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

void ABattleManager::EndBattle()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->Possess(PlayerParty[0]);
		auto player = PlayerController->GetPawn();
		if (player)
		{
			player->EnableInput(PlayerController);
			PlayerController->SetViewTargetWithBlend(player, 1.0f);
		}
	}

	DisableInput(GetWorld()->GetFirstPlayerController());
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

void ABattleManager::DisableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IMC_BattleManager)
			{
				Subsystem->RemoveMappingContext(IMC_BattleManager);
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
			Cast<AEnemy>(Character)->OnHPChanged.AddDynamic(this, &ABattleManager::OnCharacterHPChanged);
			Character->SetActorLocation(BattleFieldComp->EnemyPos->GetComponentLocation());
		}
		else if(Character && Character->ActorHasTag(FName("Player")))
		{
			PlayerParty.Add(Character);
			Cast<APlayerBase>(Character)->OnHPChanged.AddDynamic(this, &ABattleManager::OnCharacterHPChanged);
			Cast<APlayerBase>(Character)->OnUseAPDelegate.AddDynamic(this, &ABattleManager::ABattleManager::OnPlayerAPChanged);
			Character->SetActorLocation(BattleFieldComp->PlayerPos1->GetComponentLocation());
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
	//아이템
	//(X=-45704.661145,Y=16074.760444,Z=-22519.605725)
	//(Pitch = 12.200000, Yaw = -322.600005, Roll = 0.000000)
	auto player = Cast<APlayerBase>(currentCharacter);
	switch (NewState)
	{
	case EBattleState::StartBattle:
		break;
	case EBattleState::SelectAction:
		{
			player->fsm->SetCommandedState(ECommandedPlayerState::SelectAction);
			
			//FVector CamLocation = FVector(currentCharacter->GetActorLocation()) - FVector(150, -200, 0); // 목표 위치 계산
			//FVector CamLocation = currentCharacter->GetActorLocation() - FVector(-110, 80, -100); // 목표 위치 계산
			FVector CamLocation = BattleFieldComp->PlayerPos1->GetComponentLocation() - FVector(-110, 80, -100); // 목표 위치 계산
			FRotator CamRotation = FRotator(0, -200, -5); // 목표 회전 계산
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			break;
		}
	case EBattleState::SelectSkill:
		{
			player->fsm->SetCommandedState(ECommandedPlayerState::SelectSkill);
			FVector CamLocation = FVector(-45439.000000,16065.000000,-22417.495955); // 목표 위치 계산
			FRotator CamRotation = FRotator(-15, -170, 5);
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
			FVector CamLocation = EnemyParty[0]->GetActorLocation() - FVector(-500,0, -300);
			FRotator CamRotation = FRotator(0, 180, 0); 
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			//BattleCameraComp->MoveCameraTo(EnemyParty[0]->GetActorLocation() - FVector(150,0, -50), FRotator(0));
			break;
		}
	case EBattleState::PlayerPlayAction:
		{
			if (SelectedSkillIndex == 1 || SelectedSkillIndex == 0)
			{
				player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
				BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);
				BattleCameraComp->MainCamera->SetWorldLocation(FVector(-45341.000000,15607.000000,-22266.146209));
				BattleCameraComp->MainCamera->SetWorldRotation(FRotator(-9.000000,148.000000,0.004959));
				FVector CamLocation = FVector(-46117.000000,15583.000000,-22460.000000);
				FRotator CamRotation = FRotator(4.000000,148.000000,0.000000); 
				BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.f);
				//BattleCameraComp->MoveCameraTo(FVector(-170, 340, 150), FRotator(0, -40, 0));
				break;
			}
			
			if (SelectedSkillIndex == 2)
			{
				player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
				BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);
				//(X=-45448.102888,Y=16218.238706,Z=-22545.264764)
				//(Pitch=28.000000,Yaw=-154.000002,Roll=0.000000)
				// (X=-45304.936031,Y=15897.203615,Z=-22335.620916)
				// (Pitch=-14.800000,Yaw=-199.400003,Roll=-0.000000)
				BattleCameraComp->MainCamera->SetWorldLocation(FVector(-45309.274877, 16025.150328, -22494.924233));
				BattleCameraComp->MainCamera->SetWorldRotation(FRotator(4.000000,  179.400003, 0.000000));
				// 
				// 
				FVector CamLocation1 = FVector(-45448.102888, 16218.238706, -22545.264764);
				FRotator CamRotation1 = FRotator(28.000000, -154.000002, 0.000000);
				FVector CamLocation2 = FVector(-45304.936031, 15897.203615, -22335.620916);
				FRotator CamRotation2 = FRotator(-14.800000, -199.400003, -0.000000);
				BattleCameraComp->StartMoveWithTwoPointsLinear(CamLocation1, CamRotation1, 1.f, CamLocation2, CamRotation2, 0.8f);
				break;
			}
		}
	case EBattleState::EnemyPlayAction:
	{
		AEnemy* CurrentEnemy = Cast<AEnemy>(currentCharacter);
		if (CurrentEnemy)
		{
			CurrentEnemy->OnParryStart.AddDynamic(this, &ABattleManager::HandleParryStart);
			CurrentEnemy->OnParryEnd.AddDynamic(this, &ABattleManager::HandleParryEnd);
			// 1. 적 FSM에게 공격 상태로 전환하라고 직접 명령합니다.
			//    (EnemyFSM의 Tick 로직 대신 BattleManager가 흐름을 제어)
			if (CurrentEnemy->fsm)
			{
				CurrentTargetPlayer = Cast<APlayerBase>(PlayerParty[0]);
				CurrentEnemy->fsm->SetTargetToMove(CurrentTargetPlayer->GetActorLocation());
				CurrentEnemy->fsm->SetEnemyState(EEnemyState::Move);
			}

			// 2. 공격 애니메이션 길이를 가져옵니다.
			float AttackAnimLength = 10.f;
			// if (CurrentEnemy->attackAnim) // AEnemy에 기본 공격 몽타주가 할당되어 있다고 가정
			// {
			// 	AttackAnimLength = CurrentEnemy->attackAnim->GetPlayLength();
			// }

			// 3. 애니메이션 길이가 끝나면 턴을 넘기도록 타이머를 설정합니다.

			FVector CamLocation = FVector(-45380.000000,15804.000000,-22563.495955);
			FRotator CamRotation = FRotator(20.000000,141.000000,0.000000);

			//topdown
			//(X = -45402.000000, Y = 15669.000000, Z = -22253.495955)
			//(Pitch = -15.000000, Yaw = 132.000000, Roll = 0.000000)
			//HardAttack
			// (X=-45426.206194,Y=16097.681515,Z=-22482.911047)
			//(Pitch = 9.000000, Yaw = -160.200002, Roll = 0.000000)
			//(X=-45135.098347,Y=16283.588973,Z=-22556.056840)
			//(Pitch=15.800000,Yaw=-156.000002,Roll=0.000000)
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 2.0f);
			
			FTimerHandle EnemyTurnTimer;
			GetWorld()->GetTimerManager().SetTimer(
				EnemyTurnTimer,
				this,
				&ABattleManager::OnEnemyActionFinished, // 턴 넘기는 함수
				AttackAnimLength, // 애니메이션 길이만큼 기다림
				false);
		}
		break;
	}
	case EBattleState::Waiting: // 임시 카운터 스테이트
		player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
		player->fsm->ExecuteSkill(EnemyParty[0]->GetActorLocation(), 5);
		BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);
		BattleCameraComp->MainCamera->SetWorldLocation(FVector(-1219.683634, 261.927081, 406.573137));
		BattleCameraComp->MainCamera->SetWorldRotation(FRotator(-11.200000, -5.600000, 0.000000));
		FVector CamLocation = FVector(200.351082,239.330103,127.676547);
		FRotator CamRotation = FRotator(2.200000,-2.800000,0.000000); 
		BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 8.f);
		break;
	case EBattleState::EndBattle:
		Cast<APlayerBase>(PlayerParty[0])->fsm->ExitCombatMode();
		Cast<AEnemy>(EnemyParty[0])->fsm->SetEnemyState(EEnemyState::Idle);
		EndBattle();
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

	auto* Character = Cast<APlayerBase>(BattleTurnComp->GetCurrentTurnCharacter());
	if (Character)
	{
		Character->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
		Character->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandlePlayerAttackHit);
		if (Cast<AEnemy>(EnemyParty[0])->getEnemyHP() <= 0)
		{
			BattleFSMComp->ChangeState(EBattleState::EndBattle);
			return;
		}
	}

	BattleTurnComp->AdvanceTurn();
}

void ABattleManager::OnEnemyActionFinished()
{
	ACharacter* Character = BattleTurnComp->GetCurrentTurnCharacter();
	AEnemy* EnemyCharacter = Cast<AEnemy>(Character);

	if (EnemyCharacter)
	{
		// 에너미의 전체 행동이 끝났으므로, 여기서 구독을 해제합니다.
		EnemyCharacter->OnParryStart.RemoveDynamic(this, &ABattleManager::HandleParryStart);
		EnemyCharacter->OnParryEnd.RemoveDynamic(this, &ABattleManager::HandleParryEnd);
		//EnemyCharacter->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandleEnemyAttackHit);
		if (Cast<APlayerBase>(PlayerParty[0])->getplayerHP() <= 0)
		{
			BattleFSMComp->ChangeState(EBattleState::EndBattle);
			return;
		}
	}
	//EnemyCharacter->fsm->SetEnemyState(EEnemyState::Idle);
	
	BattleTurnComp->AdvanceTurn();
}

void ABattleManager::OnCharacterHPChanged(float CurrentHP, float MaxHP, ACharacter* DamagedActor)
{
	if (!BattleUIComp || !BattleUIComp->BattleHUDWidget) return;

	// 데미지를 입은 액터가 플레이어인지 적인지 확인
	if (DamagedActor->ActorHasTag(FName("Player")))
	{
		BattleUIComp->BattleHUDWidget->UpdatePlayerHP(CurrentHP, MaxHP);
		BattleUIComp->BattleHUDWidget->UpdatePlayerHPText(CurrentHP, MaxHP);
	}
	else if (DamagedActor->ActorHasTag(FName("Enemy")))
	{
		// TODO: 보스/적 HP바 업데이트 함수 호출
		BattleUIComp->BattleHUDWidget->UpdateBossHP(CurrentHP, MaxHP);
	}
}

void ABattleManager::OnPlayerAPChanged(int32 CurrentAP)
{
	if (!BattleUIComp || !BattleUIComp->BattleHUDWidget) return;

	BattleUIComp->BattleHUDWidget->UpdateCostBar(CurrentAP);
	BattleUIComp->BattleHUDWidget->UpdateCostText(CurrentAP);
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
		BattleTimingComp->OnPlayerInput();
		auto CurrentCharacter = Cast<APlayerBase>(PlayerParty[0]);
		if (CurrentCharacter)
		{
			CurrentCharacter->fsm->OnDodge();
		}
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
		//BattleFSMComp->ChangeState(EBattleState::SelectTarget);
		//SelectedSkillIndex = 3;
		return;
	}
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::EnemyPlayAction)
	{
		// parry
		BattleTimingComp->OnPlayerInput();
		auto CurrentCharacter =  Cast<APlayerBase>(PlayerParty[0]);
		if (CurrentCharacter)
		{
			CurrentCharacter->fsm->OnParry();
		}
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
		// 임시로 첫 번째 적을 타겟으로 지정
		CurrentTargetEnemy = Cast<AEnemy>(EnemyParty[0]); 

		if (CurrentCharacter && CurrentCharacter->fsm)
		{
			CurrentCharacter->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
			CurrentCharacter->OnAttackHitDelegate.AddDynamic(this, &ABattleManager::HandlePlayerAttackHit);
			
			if (CurrentTargetEnemy)
			{
				// AP 감소
				CurrentCharacter->fsm->ExecuteSkill(CurrentTargetEnemy->GetActorLocation(), SelectedSkillIndex);
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

void ABattleManager::OnTimingCheckResult(bool bSuccess, ETimingMode TimingMode)
{
	if (TimingMode == ETimingMode::EnemyParry)
	{
		if (bSuccess)
		{
			BattleDamageCalcComp->MultiplyDamage = 0;
			// 성공 시: 패링/회피 성공 로직
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Parry/Dodge SUCCEEDED!"));
			// auto player = Cast<APlayerBase>(PlayerParty[0]);
			// if(player)
			// {
			// 	player->fsm->OnParry(); // 혹은 OnDodge();
			// }
		}
		else
		{
			BattleDamageCalcComp->MultiplyDamage = 1;
			// 실패 시: 플레이어 피격 로직 실행
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parry/Dodge FAILED! Player takes damage."));
            
			//HandleEnemyAttackHit에 있던 로직을 여기에 적용
			if (EnemyParty[0] && PlayerParty[0])
			{
				auto AttackerEnemy = Cast<AEnemy>(EnemyParty[0]);
				auto TargetPlayer = Cast<APlayerBase>(PlayerParty[0]);
			
				if(AttackerEnemy && TargetPlayer)
				{
					// 1. 적이 사용 중인 스킬 정보를 가져옵니다.
					// (AEnemy에 현재 사용 스킬 인덱스를 저장하는 변수가 필요합니다)
					const FSkillRow* SkillData = AttackerEnemy->GetSkillRowByIndex(AttackerEnemy->skillIndex);
					if (SkillData)
					{
						// 2. 데미지 계산
						float FinalDamage = BattleDamageCalcComp->CalculateFinalDamage(AttackerEnemy, TargetPlayer, *SkillData);
				
						// 3. 플레이어에게 데미지 적용
						TargetPlayer->setplayerHP(FinalDamage, AttackerEnemy);
						TargetPlayer->fsm->OnTakeDamage();
						
					}
				}
			}
		}
	}
	else if (TimingMode == ETimingMode::PlayerAttack)
	{
		if (bSuccess)
		{
			BattleDamageCalcComp->MultiplyDamage = 2;
		}
		else
		{
			BattleDamageCalcComp->MultiplyDamage = 1;
		}
	}
}

void ABattleManager::HandleParryStart(AEnemy* Enemy)
{
	BattleTimingComp->StartParryTiming();
}

void ABattleManager::HandleParryEnd(AEnemy* Enemy)
{
	BattleTimingComp->EndParryTiming();

	//Enemy->OnParryStart.RemoveDynamic(this, &ABattleManager::HandleParryStart);
	//Enemy->OnParryEnd.RemoveDynamic(this, &ABattleManager::HandleParryEnd);
}

void ABattleManager::HandlePlayerAttackHit(APlayerBase* Attacker)
{
	if (CurrentTargetEnemy)
	{
		// 타겟 에너미의 피격 함수를 호출합니다!

		const FSkillRow* SkillData = Attacker->fsm->GetSkillRowByIndex(SelectedSkillIndex);
		if (SkillData)
		{
			// 2. 데미지 계산 컴포넌트에게 최종 데미지 계산을 요청합니다.
			float FinalDamage = BattleDamageCalcComp->CalculateFinalDamage(Attacker, CurrentTargetEnemy, *SkillData);

			// 3. 타겟 에너미에게 계산된 데미지를 입히라고 명령합니다.
			Cast<AEnemy>(CurrentTargetEnemy)->setEnemyHP(FinalDamage, Attacker);
		}
		if (CurrentTargetEnemy->getEnemyHP() <= 0)
			CurrentTargetEnemy->EnemyDie();
		else
			CurrentTargetEnemy->EnemyDamage();
	}
}

void ABattleManager::HandleEnemyAttackHit(AEnemy* Attacker)
{
	if (CurrentTargetPlayer)
	{
		// 타겟 에너미의 피격 함수를 호출합니다!

		const FSkillRow* SkillData = Attacker->GetSkillRowByIndex(Attacker->skillIndex);
		if (SkillData)
		{
			// 2. 데미지 계산 컴포넌트에게 최종 데미지 계산을 요청합니다.
			float FinalDamage = BattleDamageCalcComp->CalculateFinalDamage(Attacker, CurrentTargetPlayer, *SkillData);

			// 3. 타겟 에너미에게 계산된 데미지를 입히라고 명령합니다.
			Cast<APlayerBase>(CurrentTargetPlayer)->setplayerHP(FinalDamage, Attacker);
		}
		//if (CurrentTargetPlayer->getplayerHP() <= 0)
			// 죽는 처리
		//else
			CurrentTargetPlayer->fsm->OnTakeDamage();
	}
}

