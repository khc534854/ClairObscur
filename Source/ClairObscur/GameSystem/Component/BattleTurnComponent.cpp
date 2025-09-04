// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnComponent.h"

#include "ClairObscur/GameSystem/BattleManager.h"


// Sets default values for this component's properties
UBattleTurnComponent::UBattleTurnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleTurnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBattleTurnComponent::SetTurnList()
{
	auto BM = Cast<ABattleManager>(GetOwner());
	BM->BattleParticipant;

	
}


// Called every frame
void UBattleTurnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	
}

