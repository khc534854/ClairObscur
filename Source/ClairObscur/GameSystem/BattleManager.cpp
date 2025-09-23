// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"

#include "AssetTypeCategories.h"
#include "CineCameraActor.h"
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
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Component/BattleFieldComponent.h"
#include "Component/BattleResultDataComponent.h"
#include "PlayerDirectory/PlayerBase.h"
#include "PlayerDirectory/PlayerFSM.h"
#include "Widget/SelectSkillWidget.h"
#include "Widget/BattleHUDWidget.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyFSM.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

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
	BattleResultComp = CreateDefaultSubobject<UBattleResultDataComponent>(TEXT("BattleResultData"));
	
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
	
	BattleUIComp->BattleHUDWidget->AddToViewport();
	
	for (auto* player : PlayerParty)
	{
		if (player->ActorHasTag(FName("Gustave")))
		{
			BattleUIComp->BattleHUDWidget->UpdateGustaveCostBar(player->currentAP);
			BattleUIComp->BattleHUDWidget->UpdateGustaveHP(player->maxHP, player->maxHP);
			BattleUIComp->BattleHUDWidget->UpdateGustaveHPText(player->maxHP, player->maxHP);
			
			player->SetActorLocation(BattleFieldComp->PlayerPos1->GetComponentLocation());
		}
		else if (player->ActorHasTag(FName("Lune")))
		{
			BattleUIComp->BattleHUDWidget->UpdateLuneCostBar(player->currentAP);
			BattleUIComp->BattleHUDWidget->UpdateLuneHP(player->maxHP, player->maxHP);
			BattleUIComp->BattleHUDWidget->UpdateLuneHPText(player->maxHP, player->maxHP);
		}
	}

	// 전투 결과 업데이트를 위한 함수 호출
	BattleResultComp->StartBattle(); 
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

	CurrentActionActor = BattleTurnComp->GetCurrentTurnCharacter();
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
			FVector CamLocation = CurrentTargetEnemy->GetActorLocation() - FVector(-500,0, -300);
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
				//auto parryPlayer = PlayerParty[EnemyTargetIndex];
				if (CurrentTargetPlayer->ActorHasTag(FName("Gustave")))
				{
					CurrentTargetPlayer->fsm->SetCommandedState(ECommandedPlayerState::Attack);
					FVector CamLocation = FVector(-45138.781190, 16307.293779, -22082.548800);
					FRotator CamRotation = FRotator(-26.600000, -160.000002, 0.000000);
					BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 10.f);
					break;
				}
				else if (CurrentTargetPlayer->ActorHasTag(FName("Lune")))
				{
					CurrentTargetPlayer->fsm->SetCommandedState(ECommandedPlayerState::Attack);
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
					if (CurrentTargetPlayer->fsm->GetCommandedState() != ECommandedPlayerState::Die)
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
				}

				FVector CamLocation;
				FRotator CamRotation;
				if (CurrentTargetPlayer->ActorHasTag(FName("Gustave")))
				{
					if (skillIndexToUse == 3)
					{
						BattleCameraComp->MainCamera->SetWorldLocation(FVector(-45704.492189,16119.277105,-22062.213783));
						BattleCameraComp->MainCamera->SetWorldRotation(FRotator(-11.600000,200.800003,0.000000));
						CamLocation = FVector(-44903.472172,16302.224800,-22040.096421);
						CamRotation = FRotator(-24.400000,-161.000002,0.000000);
					}
					else
					{
						CamLocation = FVector(-45464.572718,15650.510706,-22154.158833);
						CamRotation = FRotator(-22.600000,-238.600004,0.000000);
					}

				}
				else if (CurrentTargetPlayer->ActorHasTag(FName("Lune")))
				{
					if (skillIndexToUse == 3)
					{
						BattleCameraComp->MainCamera->SetWorldLocation(FVector(-45639.727037,15703.430451,-22134.687635));
						BattleCameraComp->MainCamera->SetWorldRotation(FRotator(-21.000000,-177.600003,0.000000));
						CamLocation = FVector(-44946.279829,15673.720716,-22032.814928);
						CamRotation = FRotator(-25.000000,-182.800003,0.000000);
					}
					else
					{
						CamLocation = FVector(-45385.215001,15349.028405,-22390.926507);
						CamRotation = FRotator(-7.000000,124.200002,-1.000000);
					}
				}
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

			CurrentTargetEnemy->fsm->OnEnemyActionFinished.AddDynamic(this, &ABattleManager::OnEnemyActionFinished);
		}
		break;
	}
	case EBattleState::Waiting: // 임시 카운터 스테이트
		{
			CurrentActionActor = CurrentTargetPlayer;
			//auto parryplayer = PlayerParty[EnemyTargetIndex];
			BattleDamageCalcComp->MultiplyDamage = 1.f;
			SelectedSkillIndex = 4;
			if (CurrentTargetPlayer && CurrentTargetPlayer->fsm)
			{
				CounterPlayer = CurrentTargetPlayer;
				CurrentTargetEnemy->fsm->bCounterAttackIng = true;
				//parryplayer->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
				//parryplayer->OnAttackHitDelegate.AddDynamic(this, &ABattleManager::HandlePlayerAttackHit);

				//PlayerParty[0]->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
				//PlayerParty[0]->OnAttackHitDelegate.AddDynamic(this, &ABattleManager::HandlePlayerAttackHit);
				
				//PlayerParty[1]->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
				//PlayerParty[1]->OnAttackHitDelegate.AddDynamic(this, &ABattleManager::HandlePlayerAttackHit);
				
				CounterPlayer->fsm->OnSkillSequenceCompleted.AddDynamic(this, &ABattleManager::OnPlayerActionFinished);
				CounterPlayer->OnAttackHitDelegate.AddDynamic(this, &ABattleManager::HandlePlayerAttackHit);
				CounterPlayer->fsm->ExecuteSkill(CurrentTargetEnemy->GetActorLocation(), SelectedSkillIndex);
				BattleFSMComp->ChangeState(EBattleState::PlayerPlayAction);
			}
			break;
		}
	case EBattleState::EndWinBattle:
		{
			ALevelSequenceActor* SequenceActor = nullptr;                                                                        
			for (TActorIterator<ALevelSequenceActor> It(GetWorld()); It; ++It)                                                   
      		{                                                                                                                    
      		    if (It->ActorHasTag(TEXT("WinCinematic")))                                                                   
      		    {                                                                                                                
      		        SequenceActor = *It;                                                                                         
      		        break;                                                                                                       
      		    }                                                                                                                
      		}                                                                                                                    
                                                                                                                           
      	bool bPlayedSequence = false;                                                                                        
      	if (SequenceActor)                                                                                                   
      	{
      		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
      		if (PlayerController)
      		{
      			ACineCameraActor* CinematicCamera = nullptr;
      			for (TActorIterator<ACineCameraActor> It(GetWorld()); It; ++It)
      			{
      				if (It->ActorHasTag(FName("Cinecamera")))
      				{
      					CinematicCamera = *It;
      					break;
      				}
      			}

      			if (CinematicCamera)
      			{
      				if (PlayerController)
      				{
      					PlayerController->SetViewTargetWithBlend(CinematicCamera, 0.5f);
      				}
      			}
      			// 2. 플레이어의 시점을 다른 카메라가 아닌, 'LevelSequenceActor' 자체로 전환합니다.
      			//PlayerController->SetViewTargetWithBlend(SequenceActor, 0.5f);
      		}
      		
      	    if (ULevelSequencePlayer* EndPlayer = SequenceActor->GetSequencePlayer())                                           
      	    {                                                                                                                
      	        EndPlayer->OnFinished.AddDynamic(this, &ABattleManager::HandleWinSequenceFinished);                             
      	        EndPlayer->Play();                                                 
      	        EndSequenceActor  = SequenceActor;
      	        EndSequencePlayer = EndPlayer;                                                                                 
      	        bPlayedSequence   = true;                                                                                    
      	    }                                                                                                                
      	}                                                                                                                    
      	if (!bPlayedSequence)                                                                                                
      	    HandleWinSequenceFinished();                                                                                     
      	break; 
		}
	case EBattleState::EndLoseBattle:
		{
			break;
		}
	case EBattleState::NotBattle:
		break;
	case EBattleState::PhaseChange:
		{
			ALevelSequenceActor* SequenceActor = nullptr;                                                                        
			for (TActorIterator<ALevelSequenceActor> It(GetWorld()); It; ++It)                                                   
      		{                                                                                                                    
      		    if (It->ActorHasTag(TEXT("PhaseCinematic")))                                                                   
      		    {                                                                                                                
      		        SequenceActor = *It;                                                                                         
      		        break;                                                                                                       
      		    }                                                                                                                
      		}                                                                                                                    
                                                                                                                           
      		bool bPlayedSequence = false;                                                                                        
      		if (SequenceActor)                                                                                                   
      		{
      			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
      			if (PlayerController)
      			{
      				PlayerController->SetViewTargetWithBlend(SequenceActor, 0.5f);
      			}
      			
      		    if (ULevelSequencePlayer* PhasePlayer = SequenceActor->GetSequencePlayer())                                           
      		    {                                                                                                                
      		        PhasePlayer->OnFinished.AddDynamic(this, &ABattleManager::HandlePhaseSequenceFinished);                             
      		        PhasePlayer->Play();                                                 
      		        bPlayedSequence   = true;                                                                                    
      		    }                                                                                                                
      		}                                                                                                                    
      		if (!bPlayedSequence)                                                                                                
      		    HandlePhaseSequenceFinished();   
				
			break;
		}
	default:
		break;
	}
}

void ABattleManager::OnPlayerActionFinished(int SkillIndex, bool bInterrupted, bool bReachedSpot)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Action Finished. Advancing to next turn."));

	//auto* Character = Cast<APlayerBase>(BattleTurnComp->GetCurrentTurnCharacter());
	auto* Character = Cast<APlayerBase>(CurrentActionActor);
	if (Character && Character != CounterPlayer)
	{
		Character->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
		Character->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandlePlayerAttackHit);
		if (CurrentTargetEnemy->getEnemyHP() <= 0)
		{
			if (!CurrentTargetEnemy->bPhaseTwo)
			{
				BattleFSMComp->ChangeState(EBattleState::PhaseChange);
				return;
			}
			BattleFSMComp->ChangeState(EBattleState::EndWinBattle);
			return;
		}
	}

	if (BattleFSMComp->GetBeforeState() == EBattleState::Waiting && CounterPlayer)
	{
		CounterPlayer->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
		CounterPlayer->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandlePlayerAttackHit);
		//PlayerParty[0]->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
		//PlayerParty[0]->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandlePlayerAttackHit);
		//PlayerParty[1]->fsm->OnSkillSequenceCompleted.RemoveDynamic(this, &ABattleManager::OnPlayerActionFinished);
		//PlayerParty[1]->OnAttackHitDelegate.RemoveDynamic(this, &ABattleManager::HandlePlayerAttackHit);
		CounterPlayer = nullptr;

		CurrentTargetEnemy->fsm->bCounterAttackIng = false;
		
		FVector CamLocation = FVector(-45053.463949,16126.722838,-22187.798000);
		FRotator CamRotation = FRotator(-11.800000,-170.600003,0.000000);
		BattleCameraComp->StartMoveWithInterp(CamLocation, CamRotation, 1.0f);
		return;
	}
	
	BattleTurnComp->AdvanceTurn();
}

void ABattleManager::OnEnemyActionFinished()
{
	if (CurrentTargetEnemy)
	{
		// 에너미의 전체 행동이 끝났으므로, 여기서 구독을 해제합니다.
		CurrentTargetEnemy->OnParryStart.RemoveDynamic(this, &ABattleManager::HandleParryStart);
		CurrentTargetEnemy->OnParryEnd.RemoveDynamic(this, &ABattleManager::HandleParryEnd);
		CurrentTargetEnemy->fsm->OnEnemyActionFinished.RemoveDynamic(this, &ABattleManager::OnEnemyActionFinished);
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
			BattleFSMComp->ChangeState(EBattleState::EndLoseBattle);
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetWorld()->GetFirstPlayerController());
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			return;
		}

		if (CurrentTargetEnemy->fsm->bCounterAttackIng)
			return;
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

		if (CurrentTargetPlayer)
		{
			if (CurrentTargetPlayer->fsm->GetCommandedState() == ECommandedPlayerState::Die)
				return;
			CurrentTargetPlayer->fsm->OnDodge();
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
		
		if (CurrentTargetPlayer)
		{
			if (CurrentTargetPlayer->fsm->GetCommandedState() == ECommandedPlayerState::Die)
				return;
			CurrentTargetPlayer->fsm->OnParry();
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

			if (CurrentTargetEnemy && CurrentTargetPlayer)
			{
				// 1. 적이 사용 중인 스킬 정보를 가져옵니다.
				// (AEnemy에 현재 사용 스킬 인덱스를 저장하는 변수가 필요합니다)
				const FSkillRow* SkillData = CurrentTargetEnemy->GetSkillRowByIndex(CurrentTargetEnemy->skillIndex);
				if (SkillData)
				{
					// 2. 데미지 계산
					float FinalDamage = BattleDamageCalcComp->CalculateFinalDamage(CurrentTargetEnemy, CurrentTargetPlayer, *SkillData);
				
					// 3. 플레이어에게 데미지 적용
					BattleResultComp->RecordDamageReceived(FinalDamage);
					CurrentTargetPlayer->setplayerHP(FinalDamage, CurrentTargetEnemy);
					
					if (CurrentTargetPlayer->getplayerHP() <= 0)
					{
						if (CurrentTargetPlayer->ActorHasTag(FName("Gustave")))
						{
							BattleUIComp->BattleHUDWidget->SetGustaveDeathMask(true);
			
						}
						else if (CurrentTargetPlayer->ActorHasTag(FName("Lune")))
						{
							BattleUIComp->BattleHUDWidget->SetLuneDeathMask(true);

						}
						CurrentTargetPlayer->fsm->SetCommandedState(ECommandedPlayerState::Die);
						return;
					}
					
					CurrentTargetPlayer->fsm->OnTakeDamage();
					
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
			BattleResultComp->RecordDamage(FinalDamage);
			CurrentTargetEnemy->setEnemyHP(FinalDamage, Attacker);
		}
		if (CurrentTargetEnemy->getEnemyHP() <= 0 && CurrentTargetEnemy->bPhaseTwo)
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
			CurrentTargetPlayer->setplayerHP(FinalDamage, Attacker);
			

		}

		CurrentTargetPlayer->fsm->OnTakeDamage();
	}
}

void ABattleManager::HandleWinSequenceFinished()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetWorld()->GetFirstPlayerController());
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	BattleUIComp->ShowWinWidgetIfReady();
}

void ABattleManager::HandlePhaseSequenceFinished()
{
	CurrentTargetEnemy->bPhaseTwo = true;
	CurrentTargetEnemy->currentHP = CurrentTargetEnemy->maxHP;
	BattleUIComp->BattleHUDWidget->UpdateBossHP(1, 1);
	BattleTurnComp->AdvanceTurn();
}

