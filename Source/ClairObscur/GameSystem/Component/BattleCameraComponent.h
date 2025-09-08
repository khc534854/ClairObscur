// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BattleCameraComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UBattleCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBattleCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnFSMStateChanged(EBattleState NewState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* MainCamera;

	FVector TargetLocation;
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraInterpSpeed = 5.0f;
};
