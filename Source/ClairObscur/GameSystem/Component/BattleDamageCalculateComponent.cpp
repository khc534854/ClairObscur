// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleDamageCalculateComponent.h"
#include "CharacterComponent/SkillRow.h"


// Sets default values for this component's properties
UBattleDamageCalculateComponent::UBattleDamageCalculateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleDamageCalculateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBattleDamageCalculateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UBattleDamageCalculateComponent::CalculateFinalDamage(ACharacter* Attacker, ACharacter* Target,
	const struct FSkillRow& SkillData)
{
	float FinalDamage = SkillData.BasicDamage;

	// 예: float AttackerPower = Attacker->GetStatsComponent()->GetAttackPower();
	// 예: float TargetDefense = Target->GetStatsComponent()->GetDefense();
	// FinalDamage = SkillData.BasicDamage + AttackerPower - TargetDefense;

	return FinalDamage;
}

