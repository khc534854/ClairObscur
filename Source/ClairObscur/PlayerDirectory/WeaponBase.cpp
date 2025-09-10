// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDirectory/WeaponBase.h"
#include "Components/BoxComponent.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	attachpoint =CreateDefaultSubobject<USceneComponent>(TEXT("Attachpoint"));
	SetRootComponent(attachpoint);
	WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);
	WeaponMeshComp->SetupAttachment(RootComponent);

	// 평소에는 비활성화
	BoxComp->SetGenerateOverlapEvents(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	WeaponMeshComp->SetGenerateOverlapEvents(false);
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	

	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

