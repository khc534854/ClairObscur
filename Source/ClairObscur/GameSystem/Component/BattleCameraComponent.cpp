// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "BattleFSMComponent.h"
#include "BattleTurnComponent.h"
#include "ClairObscur/GameSystem/BattleManager.h" 
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UBattleCameraComponent::UBattleCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(this);
}


// Called when the game starts
void UBattleCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBattleCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsMoving && UpdateCameraMovement)
	{
		UpdateCameraMovement(DeltaTime);
	}
}

void UBattleCameraComponent::SetFocusOnCharacter(const ACharacter* TargetCharacter)
{
	if (!TargetCharacter) return;

	const FVector Offset = (TargetCharacter->GetActorForwardVector() * -300.0f) + (TargetCharacter->GetActorRightVector() * 150.0f) + FVector(0, 0, 100.0f);
	TargetLocation = TargetCharacter->GetActorLocation() + Offset;

	// 캐릭터를 바라보도록 회전값을 계산합니다.
	TargetRotation = (TargetCharacter->GetActorLocation() - TargetLocation).Rotation();
}

void UBattleCameraComponent::SetWideShot(const TArray<ACharacter*>& AllCharacters)
{
}

void UBattleCameraComponent::StartMoveWithInterp(FVector NewTargetLocation, FRotator NewTargetRotation,
	float InterpSpeed)
{
	TargetLocation = NewTargetLocation;
	TargetRotation = NewTargetRotation;
	CameraInterpSpeed = InterpSpeed;
    
	// Tick에서 UpdateWithInterp 함수를 실행하도록 '전략'을 설정합니다.
	UpdateCameraMovement = [this](float DeltaTime) { this->UpdateWithInterp(DeltaTime); };
    
	bIsMoving = true;
}

void UBattleCameraComponent::StartMoveWithCurve(FVector NewTargetLocation, FRotator NewTargetRotation, float Duration)
{
	StartLocation = MainCamera->GetComponentLocation();
	StartRotation = MainCamera->GetComponentRotation();
	TargetLocation = NewTargetLocation;
	TargetRotation = NewTargetRotation;
	MovementDuration = Duration;
	MovementElapsedTime = 0.0f;
    
	// Tick에서 UpdateWithCurve 함수를 실행하도록 '전략'을 설정합니다.
	UpdateCameraMovement = [this](float DeltaTime) { this->UpdateWithCurve(DeltaTime); };

	bIsMoving = true;
}

void UBattleCameraComponent::MoveCameraTo(FVector NewTargetLocation, FRotator NewTargetRotation)
{
	TargetLocation = NewTargetLocation;
	TargetRotation = NewTargetRotation;
}

void UBattleCameraComponent::UpdateWithInterp(float DeltaTime)
{
	const FVector CurrentLocation = MainCamera->GetComponentLocation();
	const FRotator CurrentRotation = MainCamera->GetComponentRotation();
    
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, CameraInterpSpeed);
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, CameraInterpSpeed);
	MainCamera->SetWorldLocationAndRotation(NewLocation, NewRotation);

	// 목표 지점에 거의 도달하면 이동을 멈춥니다.
	if (CurrentLocation.Equals(TargetLocation, 1.0f))
	{
		bIsMoving = false;
	}
}

void UBattleCameraComponent::UpdateWithCurve(float DeltaTime)
{
	MovementElapsedTime += DeltaTime * CameraInterpSpeed;
	const float TimeRatio = FMath::Clamp(MovementElapsedTime / MovementDuration, 0.0f, 1.0f);
	const float CurveAlpha = MovementCurve->GetFloatValue(TimeRatio);

	const FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, CurveAlpha);
	const FQuat NewRotationQuat = FQuat::Slerp(StartRotation.Quaternion(), TargetRotation.Quaternion(), CurveAlpha);
	MainCamera->SetWorldLocationAndRotation(NewLocation, NewRotationQuat);

	if (MovementElapsedTime >= MovementDuration)
	{
		bIsMoving = false;
	}
}

