// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleFieldComponent.h"


// Sets default values for this component's properties
UBattleFieldComponent::UBattleFieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	//
	// BaseComp =   CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseComp"));
	// BaseComp->SetRelativeScale3D(FVector(30.0f, 30.0f, 1.0f));
	
	PlayerPos1 = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerPos1"));
	PlayerPos1->SetRelativeLocation(FVector(-30.0f, -25.0f, 60.5f));
	PlayerPos2 = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerPos2")); 
	PlayerPos2->SetRelativeLocation(FVector(-30.0f, 25.0f, 60.5f));
	EnemyPos =   CreateDefaultSubobject<USceneComponent>(TEXT("EnemyPos"));
	EnemyPos->SetRelativeLocation(FVector(50.0f, 0.0f, 100.5f));
	
}


// Called when the game starts
void UBattleFieldComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBattleFieldComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

