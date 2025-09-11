// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyAnimInstance.h"
#include "EnemyFSM.h"

#include "CharacterComponent/SkillRow.h"

//HP

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*enemySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("enemySkeletalMesh"));
	enemySkeletalMesh->SetupAttachment(GetRootComponent());*/

	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>TempEnemySkeletal(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Skins/Tier_1_5/MaskedReaper/Mesh/Sevarog_MaskedReaper_GDC.Sevarog_MaskedReaper_GDC'"));

	if (TempEnemySkeletal.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempEnemySkeletal.Object);
		GetMesh()->SetRelativeLocation(FVector(0,0,0));
		GetMesh()->SetRelativeScale3D(FVector(1));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		static ConstructorHelpers::FClassFinder<UAnimInstance>TempAnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Sevarog_AnimBlueprint.Sevarog_AnimBlueprint_C'"));

		if (TempAnimBP.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(TempAnimBP.Class);
		}
	}

	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackMontage(TEXT("/Script/Engine.AnimMontage'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Animations/Swing1_Medium_Montage.Swing1_Medium_Montage'"));
	if (attackMontage.Succeeded())
	{
		attackAnim = attackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> damageMontage(TEXT("/Script/Engine.AnimMontage'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Animations/Hitreact_Front_Montage.Hitreact_Front_Montage'"));
	if (damageMontage.Succeeded())
	{
		damageAnim = damageMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> dieMontage(TEXT("/Script/Engine.AnimMontage'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Animations/Death_front_Montage.Death_front_Montage'"));
	if (dieMontage.Succeeded())
	{
		dieAnim = dieMontage.Object;
	}

	Tags.Add(FName("Enemy"));
	Tags.Add(FName("BattlePossible"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh() && GetMesh()->GetAnimInstance())
	{

		AnimInst = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInst)
		{
			AnimInst->ownerEnemy = this; // 자기 자신 연결
			
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::EnemyIdle()
{
}

void AEnemy::EnemyMove(FVector destination)
{
	auto target = GetWorld()->GetFirstPlayerController();
	
}

void AEnemy::EnemyAttack()
{
	EnemySkill(FVector(0), 2);
}

void AEnemy::EnemyDamage()
{
	if (damageAnim && GetMesh())
	{	
		if (AnimInst)
		{
			float Duration = AnimInst->Montage_Play(damageAnim);

			if (Duration > 0.f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Montage %s is playing!"), *damageAnim->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to play montage %s"), *damageAnim->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No AnimInstance found on enemySkeletalMesh!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No montage assigned!"));
	}
}

void AEnemy::EnemyDie()
{
	if (dieAnim && GetMesh())
	{	
		if (AnimInst)
		{
			float Duration = AnimInst->Montage_Play(dieAnim);
			currentTime += GetWorld()->DeltaTimeSeconds;
			if (currentTime > Duration)
				Destroy();

			if (Duration > 0.f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Montage %s is playing!"), *dieAnim->GetName());
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemy::DestroySelf, Duration, false);
			
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to play montage %s"), *dieAnim->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No AnimInstance found on enemySkeletalMesh!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No montage assigned!"));
	}
	
	
}

void AEnemy::EnemySkill(const FVector& TargetLocation, int32 SkillIndex)
{
	if (const FSkillRow* Row = GetSkillRowByIndex(SkillIndex))
	{
		if (UAnimMontage* Montage = Row->SkillMontage.LoadSynchronous())
		{
			if (AnimInst)
			{
				float Duration = AnimInst->Montage_Play(Montage);

				if (Duration > 0.f)
				{
					UE_LOG(LogTemp, Warning, TEXT("Playing montage from DataTable: %s"), *Montage->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to play montage %s"), *Montage->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No AnimInstance on mesh!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SkillRow %d has no valid montage!"), SkillIndex);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid SkillIndex %d"), SkillIndex);
	
	}

}

/*
{
	if (const FSkillRow* Row = GetSkillRowByIndex(SkillIndex))
	{
		if (UAnimMontage* M = Row->SkillMontage.LoadSynchronous())
		{
			if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
			{
				// 끝나면 복귀 시작
				Anim->Montage_Play(M);
				return; // 복귀는 몽타주 끝나고
			}
		}
	}
}
*/

const FSkillRow* AEnemy::GetSkillRowByIndex(int32 Index) const
{
	UDataTable* Table = SkillTable.LoadSynchronous();
	if (!Table){return nullptr;}

	// 매 호출마다 RowNames 가져오기 
	const TArray<FName> RowNames = Table->GetRowNames();
	if (!RowNames.IsValidIndex(Index)) {return nullptr;}

	static const FString Ctx = TEXT("GetSkillRowByIndex");
	return Table->FindRow<FSkillRow>(RowNames[Index], Ctx);
}

void AEnemy::DestroySelf()
{
	Destroy();
}

/*void AEnemy::OnEnemyNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "ParryWindow_Begin")
	{
		// 예: 공격 판정, 막기 가능 등 처리
		bCanBeParried = true;
		fsm->OnParryWindowOpened();
	}
}

void AEnemy::OnEnemyNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "ParryWindow_End")
	{
		// 막기 종료 처리
		bCanBeParried = false;
		fsm->OnParryWindowClosed();
	}
}*/


