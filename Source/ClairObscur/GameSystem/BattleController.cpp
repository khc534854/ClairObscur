// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleController.h"

#include "BattleManager.h"
#include "Kismet/GameplayStatics.h"

void ABattleController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("EnterBattle", IE_Pressed, this, &ABattleController::OnStartBattle);
}

void ABattleController::OnStartBattle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Enter Pressed: Switching input to BattleManager..."));

	APawn* CurrentPawn = GetPawn();
	if (CurrentPawn)
	{
		CurrentPawn->DisableInput(this);
	}

	ABattleManager* BattleManager = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleManager::StaticClass()));
	if (BattleManager)
	{
		BattleManager->EnableInput(this);
	}
}