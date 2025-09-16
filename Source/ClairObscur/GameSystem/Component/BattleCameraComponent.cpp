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

void UBattleCameraComponent::StartMoveWithTwoPointsLinear(FVector InTargetLocation1, FRotator InTargetRotation1, float InDuration1, FVector InTargetLocation2, FRotator InTargetRotation2, float InDuration2)
{
	StartLocation = MainCamera->GetComponentLocation();
	StartRotation = MainCamera->GetComponentRotation();
	TargetLocation1 = InTargetLocation1;
	TargetRotation1 = InTargetRotation1;
	Duration1 = FMath::Max(KINDA_SMALL_NUMBER, InDuration1); // 0으로 나누기 방지

	// 2단계 이동 정보 저장
	TargetLocation2 = InTargetLocation2;
	TargetRotation2 = InTargetRotation2;
	Duration2 = FMath::Max(KINDA_SMALL_NUMBER, InDuration2);

	// 상태 초기화
	MovementElapsedTime = 0.0f;
	CurrentMovePhase = 1; // 1단계부터 시작
	bIsMoving = true;

	UpdateCameraMovement = [this](float DeltaTime) { this->UpdateWithTwoPointsLinear(DeltaTime); };
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

void UBattleCameraComponent::UpdateWithTwoPointsLinear(float DeltaTime)
{
	MovementElapsedTime += DeltaTime; // 경과 시간 증가

	if (CurrentMovePhase == 1)
	{
		// --- 1단계: 첫 번째 목표 지점으로 이동 ---
		const float Alpha = FMath::Clamp(MovementElapsedTime / Duration1, 0.0f, 1.0f);

		const FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation1, Alpha);
		const FQuat NewRotation = FQuat::Slerp(StartRotation.Quaternion(), TargetRotation1.Quaternion(), Alpha);
		MainCamera->SetWorldLocationAndRotation(NewLocation, NewRotation); // FQuat 버전을 사용하는 것이 더 좋습니다.

		// 1단계 완료 시
		if (MovementElapsedTime >= Duration1)
		{
			CurrentMovePhase = 2; // 2단계로 전환
			MovementElapsedTime = 0.0f; // 타이머 리셋

			// 2단계의 시작점을 1단계의 정확한 목표점으로 갱신
			StartLocation = TargetLocation1;
			StartRotation = TargetRotation1;
		}
	}
	else if (CurrentMovePhase == 2)
	{
		// --- 2단계: 두 번째 목표 지점으로 이동 ---
		const float Alpha = FMath::Clamp(MovementElapsedTime / Duration2, 0.0f, 1.0f);

		// 1단계의 끝(StartLocation)에서 2단계 목표(TargetLocation2)로 보간
		const FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation2, Alpha);
		const FQuat NewRotation = FQuat::Slerp(StartRotation.Quaternion(), TargetRotation2.Quaternion(), Alpha);
		MainCamera->SetWorldLocationAndRotation(NewLocation, NewRotation);

		// 2단계(모든 이동) 완료 시
		if (MovementElapsedTime >= Duration2)
		{
			bIsMoving = false; // 전체 이동 종료
			UpdateCameraMovement = nullptr; // 실행 함수 초기화 (중요)
			CurrentMovePhase = 0; // 상태 초기화
		}
	}
}

