// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDirectory/WeaponBase.h"

#include "Components/BoxComponent.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComp"));
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));

	SetRootComponent(BoxComp);
	WeaponMeshComp->SetupAttachment(RootComponent);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("PlayerWeapon"));

	WeaponMeshComp->SetGenerateOverlapEvents(false);
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

