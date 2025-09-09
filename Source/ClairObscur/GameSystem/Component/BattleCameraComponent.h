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

	/** 방법 2: 커브와 Lerp를 사용한 이동을 시작합니다. */
	void StartMoveWithCurve(FVector NewTargetLocation, FRotator NewTargetRotation, float Duration);
	
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

	void UpdateWithInterp(float DeltaTime);
	void UpdateWithCurve(float DeltaTime);
};
