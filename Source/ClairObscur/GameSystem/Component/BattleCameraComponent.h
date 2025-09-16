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

	void SetFocusOnCharacter(const ACharacter* TargetCharacter);

	void SetWideShot(const TArray<ACharacter*>& AllCharacters);

	void StartMoveWithInterp(FVector NewTargetLocation, FRotator NewTargetRotation, float InterpSpeed);

	void StartMoveWithCurve(FVector NewTargetLocation, FRotator NewTargetRotation, float Duration);

	void StartMoveWithTwoPointsLinear(
		FVector InTargetLocation1, FRotator InTargetRotation1, float InDuration1,
		FVector InTargetLocation2, FRotator InTargetRotation2, float InDuration2
	);
	
	void MoveCameraTo(FVector NewTargetLocation, FRotator NewTargetRotation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* MainCamera;

	bool bIsMoving = false;
	FVector TargetLocation;
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category = "Camera Settings|Curve")
	class UCurveFloat* MovementCurve;
	float MovementDuration;
	float MovementElapsedTime;
	FVector StartLocation;
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraInterpSpeed;

	TFunction<void(float)> UpdateCameraMovement;

private:
	void UpdateWithInterp(float DeltaTime);
	void UpdateWithCurve(float DeltaTime);
	void UpdateWithTwoPointsLinear(float DeltaTime);

	FVector TargetLocation1;
	FRotator TargetRotation1;
	float Duration1;

	FVector TargetLocation2;
	FRotator TargetRotation2;
	float Duration2;

	int32 CurrentMovePhase;
};
