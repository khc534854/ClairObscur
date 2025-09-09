// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClairObscur/GameSystem/SkillDataAsset.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionFinishedSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ExecuteSkill(int32 SkillIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkillDataAsset*> SkillDataAssets;

	UPROPERTY()
	class ACharacter* OwnerCharacter;

	UPROPERTY()
	FOnActionFinishedSignature OnActionFinished;

};