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
	//BattleFieldComp->BaseComp->SetupAttachment(BattleFieldComp);
	BattleFieldComp->PlayerPos1->SetupAttachment(BattleFieldComp);
	BattleFieldComp->PlayerPos2->SetupAttachment(BattleFieldComp);
	BattleFieldComp->EnemyPos->SetupAttachment(BattleFieldComp);
	
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
	//BattleTimingComp->OnParrySequenceFinished.AddDynamic(this, &ABattleManager::OnParrySequenceResult);
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
	for (auto* player : PlayerParty)
	{
		if (player->ActorHasTag(FName("Gustave")))
		{
			BattleUIComp->BattleHUDWidget->UpdateGustaveCostBar(player->currentAP);
			player->SetActorLocation(BattleFieldComp->PlayerPos1->GetComponentLocation());
		}
		else if (player->ActorHasTag(FName("Lune")))
		{
			BattleUIComp->BattleHUDWidget->UpdateLuneCostBar(player->currentAP);
		}
	}
	BattleUIComp->BattleHUDWidget->AddToViewport();
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
	
	BattleParticipant.SetNum(3);
	
	for (AActor* Actor : ActorsWithTag)
	{
		ACharacter* Participant = Cast<ACharacter>(Actor);
		if (Participant->ActorHasTag(FName("Gustave")))
		{
			BattleParticipant[1] = Participant;
		}
		else if (Participant->ActorHasTag(FName("Lune")))
		{
			BattleParticipant[0] = Participant;
		}
		else if (Participant->ActorHasTag(FName("Enemy")))
		{
			BattleParticipant[2] = Participant;
		}
	}

	for (ACharacter* Character : BattleParticipant)
	{
		if (Character && Character->ActorHasTag(FName("Enemy")))
		{
			EnemyParty.Add(Cast<AEnemy>(Character));
			Cast<AEnemy>(Character)->OnHPChanged.AddDynamic(this, &ABattleManager::OnCharacterHPChanged);
			Character->SetActorLocation(BattleFieldComp->EnemyPos->GetComponentLocation());
			CurrentTargetEnemy = Cast<AEnemy>(Character);
		}
		else if(Character && Character->ActorHasTag(FName("Gustave")))
		{
			PlayerParty.Add(Cast<APlayerBase>(Character));
			Cast<APlayerBase>(Character)->OnHPChanged.AddDynamic(this, &ABattleManager::OnCharacterHPChanged);
			Cast<APlayerBase>(Character)->OnUseAPDelegate.AddDynamic(this, &ABattleManager::ABattleManager::OnPlayerAPChanged);
			Character->SetActorLocation(BattleFieldComp->PlayerPos1->GetComponentLocation());
		}
		else if(Character && Character->ActorHasTag(FName("Lune")))
		{
			PlayerParty.Add(Cast<APlayerBase>(Character));
			Cast<APlayerBase>(Character)->OnHPChanged.AddDynamic(this, &ABattleManager::OnCharacterHPChanged);
			Cast<APlayerBase>(Character)->OnUseAPDelegate.AddDynamic(this, &ABattleManager::ABattleManager::OnPlayerAPChanged);
			Character->SetActorLocation(BattleFieldComp->PlayerPos2->GetComponentLocation());
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
			if (player->ActorHasTag(FName("Gustave")))
			{
				player->fsm->SetCommandedState(ECommandedPlayerState::SelectAction);
				FVector CamLocation = FVector(-45421.742144,16008.271042,-22497.130212); // 목표 위치 계산
				FRotator CamRotation = FRotator(-3.600000,165.400002,-7.000000); // 목표 회전 계산
				BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
				break;
			}
			else if (player->ActorHasTag(FName("Lune")))
			{
				player->fsm->SetCommandedState(ECommandedPlayerState::SelectAction);
				FVector CamLocation = FVector(-45409.962828,15619.690606,-22448.027798); // 목표 위치 계산
				FRotator CamRotation = FRotator(-6.200000,164.800002,-10.2); // 목표 회전 계산
				BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
				break;
			}
		}
	case EBattleState::SelectSkill:
		{
			if (player->ActorHasTag(FName("Gustave")))
			{
				player->fsm->SetCommandedState(ECommandedPlayerState::SelectSkill);
				FVector CamLocation = FVector(-45442.688278,16061.873892,-22477.463789); // 목표 위치 계산
				FRotator CamRotation = FRotator(-8.000000,183.400003,5.000000);
				BattleCameraComp->StartMoveWithCurve(CamLocation, CamRotation, 5.0f);
			}
			else if (player->ActorHasTag(FName("Lune")))
			{
				player->fsm->SetCommandedState(ECommandedPlayerState::SelectSkill);
				FVector CamLocation = FVector(-45441.226607,15677.382393,-22485.805308); // 목표 위치 계산
				FRotator CamRotation = FRotator(1.400000,184.400003,7.000000);
				BattleCameraComp->StartMoveWithCurve(CamLocation, CamRotation, 5.0f);
			}
			
			UDataTable* SkillTable = Cast<APlayerBase>(currentCharacter)->fsm->SkillTable.LoadSynchronous();
			if (SkillTable)
			{
				TArray<FSkillRow*> SkillRows;
				SkillTable->GetAllRows(TEXT(""), SkillRows);
				Cast<USelectSkillWidget>(BattleUIComp->SelectSkillWidget)->PopulateSkills(SkillRows, player);
			}
			break;
		}
	case EBattleState::SelectTarget:
		{
			FVector CamLocation = EnemyParty[0]->GetActorLocation() - FVector(-500,0, -300);
			FRotator CamRotation = FRotator(0, 180, 0); 
			BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.0f);
			break;
		}
	case EBattleState::PlayerPlayAction:
		{
			if (player)
			{
				if (player->ActorHasTag(FName("Gustave")))
				{
					//기본공격, 연속공격
					if (SelectedSkillIndex == 1 || SelectedSkillIndex == 0)
					{
						player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
						BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);
						BattleCameraComp->MainCamera->SetWorldLocation(FVector(-45230.609780,15887.098465,-22405.529680));
						BattleCameraComp->MainCamera->SetWorldRotation(FRotator(1.600000,-178.400003,0.000000));
						FVector CamLocation = FVector(-46033.214843,15591.902549,-22368.494045);
						FRotator CamRotation = FRotator(-8.000000,-197.400003,0.000000); 
						BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 5.f);
						break;
					}
					// 마법 원거리 스킬
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
					// 카운터
				}
				else if (player->ActorHasTag(FName("Lune")))
				{
					//기본공격, 연속공격
					if (SelectedSkillIndex == 1)
					{
						player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
						BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);
						BattleCameraComp->MainCamera->SetWorldLocation(FVector(-45103.628456,15684.470745,-22413.063793));
						BattleCameraComp->MainCamera->SetWorldRotation(FRotator(-6.400000,178.800003,0.000000));
				
						FVector CamLocation = FVector(-44922.846553,15702.721723,-22384.188924); // 목표 위치 계산
						FRotator CamRotation = FRotator(-6.200000,180.200003,0.000000);
						BattleCameraComp->StartMoveWithCurve(CamLocation, CamRotation, 10.0f);
						
						break;
					}
					// 마법 원거리 스킬
					if (SelectedSkillIndex == 2 || SelectedSkillIndex == 0)
					{
						player->fsm->SetCommandedState(ECommandedPlayerState::Attack);
						BattleTimingComp->StartTimingEvent(1.0f, 0.75f, 1.0f);
						FVector CamLocation = FVector(-45134.240548,15595.455399,-22430.885121);
						FRotator CamRotation = FRotator(-5.200000,-186.200003,0.000000); 
						BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 10.f);
						break;
					}
				}
			}
			else //counter
			{
				auto parryPlayer = PlayerParty[EnemyTargetIndex];
				if (parryPlayer->ActorHasTag(FName("Gustave")))
				{
					parryPlayer->fsm->SetCommandedState(ECommandedPlayerState::Attack);
					FVector CamLocation = FVector(-45602.650125,16065.516776,-22582.307046);
					FRotator CamRotation = FRotator(21.800000,-166.600002,0.000000); 
					BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 10.f);
					break;
				}
				else if (parryPlayer->ActorHasTag(FName("Lune")))
				{
					parryPlayer->fsm->SetCommandedState(ECommandedPlayerState::Attack);
					FVector CamLocation = FVector(-45224.715591,15655.399475,-22498.825878);
					FRotator CamRotation = FRotator(9.200000,-184.400003,0.000000); 
					BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 10.f);
					break;
				}
			}

		}
	case EBattleState::EnemyPlayAction:
	{
		AEnemy* CurrentEnemy = Cast<AEnemy>(currentCharacter);
		if (CurrentEnemy)
		{
			CurrentEnemy->OnParryStart.AddDynamic(this, &ABattleManager::HandleParryStart);
			CurrentEnemy->OnParryEnd.AddDynamic(this, &ABattleManager::HandleParryEnd);
			
			if (CurrentEnemy->fsm)
			{
				while (true)
				{
					EnemyTargetIndex = FMath::RandRange(0,PlayerParty.Num() - 1);
					CurrentTargetPlayer = PlayerParty[EnemyTargetIndex];
					if (CurrentTargetPlayer->getplayerHP() >= 0)
						break;				
				}
				
				CurrentEnemy->fsm->SetTargetToMove(CurrentTargetPlayer->GetActorLocation());
				CurrentEnemy->fsm->SetEnemyState(EEnemyState::Move);

				int32 skillIndexToUse = CurrentEnemy->skillIndex;
				const FSkillRow* SkillData = CurrentEnemy->GetSkillRowByIndex(skillIndexToUse);
				
				if (SkillData)
				{
					BattleTimingComp->MaxAttackCount = SkillData->AttackCount;
					BattleTimingComp->CurrentParryCount = 0;
					//BattleTimingComp->StartEnemyParrySequence(SkillData->AttackCount);
				}
			}

			if (PlayerParty[EnemyTargetIndex]->ActorHasTag(FName("Gustave")))
			{
				FVector CamLocation = FVector(-45464.572718,15650.510706,-22154.158833);
				FRotator CamRotation = FRotator(-22.600000,-238.600004,0.000000);
				BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 2.0f);
			}
			else if (PlayerParty[EnemyTargetIndex]->ActorHasTag(FName("Lune")))
			{
				FVector CamLocation = FVector(-45385.215001,15349.028405,-22390.926507);
				FRotator CamRotation = FRotator(-7.000000,124.200002,-1.000000);
				BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 2.0f);
			}

			//topdown
			//(X = -45402.000000, Y = 15669.000000, Z = -22253.495955)
			//(Pitch = -15.000000, Yaw = 132.000000, Roll = 0.000000)
			//HardAttack
			// (X=-45426.206194,Y=16097.681515,Z=-22482.911047)
			//(Pitch = 9.000000, Yaw = -160.200002, Roll = 0.000000)
			//(X=-45135.098347,Y=16283.588973,Z=-22556.056840)
			//(Pitch=15.800000,Yaw=-156.000002,Roll=0.000000)

			EnemyParty[0]->fsm->OnEnemyActionFinished.AddDynamic(this, &ABattleManager::OnEnemyActionFinished);
		}
		break;
	}
	case EBattleState::Waiting: // 임시 카운터 스테이트
		{
			auto parryplayer = PlayerParty[EnemyTargetIndex];
			SelectedSkillIndex = 4;
			CurrentTargetEnemy = EnemyParty[0];
			parryplayer->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
			parryplayer->OnAttackHitDelegate.AddDynamic(this, &ABattleManager::HandlePlayerAttackHit);
			parryplayer->fsm->ExecuteSkill(EnemyParty[0]->GetActorLocation(), SelectedSkillIndex);
			BattleFSMComp->ChangeState(EBattleState::PlayerPlayAction);
			EnemyParty[0]->fsm->bCounterAttackIng = true;
			break;
		}
	case EBattleState::EndBattle:
		{
			// for (auto players : PlayerParty)
			// {
			// 	players->fsm->ExitCombatMode();
			// }
			// EnemyParty[0]->fsm->SetEnemyState(EEnemyState::Idle);
			// EndBattle();
			break;
		}
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
		if (EnemyParty[0]->getEnemyHP() <= 0)
		{
			BattleFSMComp->ChangeState(EBattleState::EndBattle);
			return;
		}
	}

	if (BattleFSMComp->GetBeforeState() == EBattleState::Waiting)
	{
		PlayerParty[EnemyTargetIndex]->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
		PlayerParty[EnemyTargetIndex]->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandlePlayerAttackHit);
		EnemyParty[0]->fsm->bCounterAttackIng = false;
		
		FVector CamLocation = FVector(-45053.463949,16126.722838,-22187.798000);
		FRotator CamRotation = FRotator(-11.800000,-170.600003,0.000000);
		BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 1.0f);
		return;
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
		EnemyParty[0]->fsm->OnEnemyActionFinished.RemoveDynamic(this, &ABattleManager::OnEnemyActionFinished);
		//EnemyCharacter->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandleEnemyAttackHit);

		int32 DieCount = 0;
		for (auto* player : PlayerParty)
		{
			if (player->getplayerHP() <= 0)
			{
				//player->fsm->SetCommandedState(ECommandedPlayerState::Die);
				DieCount++;
			}
		}

		if (DieCount == PlayerParty.Num())
		{
			//UI 호출
			BattleFSMComp->ChangeState(EBattleState::EndBattle);
			return;
		}
	}
	
	BattleTurnComp->AdvanceTurn();
}

void ABattleManager::OnCharacterHPChanged(float CurrentHP, float MaxHP, ACharacter* DamagedActor)
{
	if (!BattleUIComp || !BattleUIComp->BattleHUDWidget) return;

	if (DamagedActor->ActorHasTag(FName("Gustave")))
	{
		BattleUIComp->BattleHUDWidget->UpdateGustaveHP(CurrentHP, MaxHP);
		BattleUIComp->BattleHUDWidget->UpdateGustaveHPText(CurrentHP, MaxHP);
	}
	else if (DamagedActor->ActorHasTag(FName("Lune")))
	{
		BattleUIComp->BattleHUDWidget->UpdateLuneHP(CurrentHP, MaxHP);
		BattleUIComp->BattleHUDWidget->UpdateLuneHPText(CurrentHP, MaxHP);
	}
	else if (DamagedActor->ActorHasTag(FName("Enemy")))
	{
		BattleUIComp->BattleHUDWidget->UpdateBossHP(CurrentHP, MaxHP);
	}
}

void ABattleManager::OnPlayerAPChanged(int32 CurrentAP, ACharacter* UseCostActor)
{
	if (!BattleUIComp || !BattleUIComp->BattleHUDWidget) return;

	if (UseCostActor->ActorHasTag(FName("Gustave")))
	{
		BattleUIComp->BattleHUDWidget->UpdateGustaveCostBar(CurrentAP);
		BattleUIComp->BattleHUDWidget->UpdateGustaveCostText(CurrentAP);
	}
	else if (UseCostActor->ActorHasTag(FName("Lune")))
	{
		BattleUIComp->BattleHUDWidget->UpdateLuneCostBar(CurrentAP);
		BattleUIComp->BattleHUDWidget->UpdateLuneCostText(CurrentAP);
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
		auto player = Cast<APlayerBase>(BattleTurnComp->GetCurrentTurnCharacter());
		if (player->getplayerAP() >= player->fsm->GetSkillRowByIndex(1)->AP )
		{
			BattleFSMComp->ChangeState(EBattleState::SelectTarget);
			SelectedSkillIndex = 1;
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough AP!!"));

	}
	
	
	if (BattleFSMComp->GetCurrentState() == EBattleState::EnemyPlayAction)
	{
		// Dodge
		BattleTimingComp->OnPlayerInputQ();
		auto CurrentCharacter = PlayerParty[EnemyTargetIndex];
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
		auto player = Cast<APlayerBase>(BattleTurnComp->GetCurrentTurnCharacter());
		if (player->getplayerAP() >= player->fsm->GetSkillRowByIndex(2)->AP )
		{
			BattleFSMComp->ChangeState(EBattleState::SelectTarget);
			SelectedSkillIndex = 2;
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough AP!!"));

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
		auto CurrentCharacter =  PlayerParty[EnemyTargetIndex];
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
		CurrentTargetEnemy = EnemyParty[0]; 

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
		}
		else
		{
			BattleDamageCalcComp->MultiplyDamage = 1;

			if (EnemyParty[0] && PlayerParty[EnemyTargetIndex])
			{
				auto AttackerEnemy = EnemyParty[0];
				auto TargetPlayer = PlayerParty[EnemyTargetIndex];
			
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
						
						if (Cast<APlayerBase>(CurrentTargetPlayer)->getplayerHP() <= 0)
						{
							Cast<APlayerBase>(CurrentTargetPlayer)->fsm->SetCommandedState(ECommandedPlayerState::Die);
							return;
						}
						
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
			BattleDamageCalcComp->MultiplyDamage = 1.2f;
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

		CurrentTargetPlayer->fsm->OnTakeDamage();
	}
}

