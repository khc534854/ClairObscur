// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	enemySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("enemySkeletalMesh"));
	enemySkeletalMesh->SetupAttachment(GetRootComponent());

	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>TempEnemySkeletal(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Skins/Tier_1_5/MaskedReaper/Mesh/Sevarog_MaskedReaper_GDC.Sevarog_MaskedReaper_GDC'"));

	if (TempEnemySkeletal.Succeeded())
	{
		enemySkeletalMesh->SetSkeletalMesh(TempEnemySkeletal.Object);
		enemySkeletalMesh->SetRelativeLocation(FVector(0,0,0));
		enemySkeletalMesh->SetRelativeScale3D(FVector(1));
		enemySkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));

		static ConstructorHelpers::FClassFinder<UAnimInstance>TempAnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Sevarog_AnimBlueprint.Sevarog_AnimBlueprint_C'"));

		if (TempAnimBP.Succeeded())
		{
			enemySkeletalMesh->SetAnimInstanceClass(TempAnimBP.Class);
		
		}
	
		
	}


}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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

