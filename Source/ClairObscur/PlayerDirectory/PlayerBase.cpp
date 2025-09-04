// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerDirectory/PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default value
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);

	// 캡슐 컴포넌트 충돌 설정
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));


	// SpringArm, Camera
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	
	// 카메라 설정 
	springArmComp -> SetupAttachment(GetMesh());
	cameraComp ->SetupAttachment(springArmComp);
	
	// 위치 설정
	springArmComp->SetRelativeLocation(FVector(0.000000,0.000000,107.192264));
	springArmComp->SetRelativeRotation(FRotator(0.000000,90.000000,0.000000));
	springArmComp->TargetArmLength= 310.f;
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->bInheritPitch= true;
	springArmComp->bInheritYaw= true;
	springArmComp->bInheritRoll= true;
	springArmComp->bInheritPitch= true;
	
	
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// 매핑
	APlayerController* pc = GetWorld()->GetFirstPlayerController();

	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsys)
		{
			subsys->AddMappingContext(IMC_Player, 0);
		}
	}

	
	MoveToFloor();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (input)
	{

		input->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &APlayerBase::MoveForward_Triggered);
		input->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &APlayerBase::MoveRight_Triggered);
		input->BindAction(IA_JogOverride, ETriggerEvent::Triggered, this, &APlayerBase::JogOverrideAction_Triggered);
		input->BindAction(IA_JogOverride, ETriggerEvent::Completed, this, &APlayerBase::JogOverrideAction_Finished);
		input->BindAction(IA_LookTurn, ETriggerEvent::Triggered, this, &APlayerBase::HandleTurnInput);
		input->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &APlayerBase::HandleLookUpInput);
		input->BindAction(IA_SpwanWeapon, ETriggerEvent::Started, this, &APlayerBase::OnToggleCombat_Triggered);
		input->BindAction(IA_AttackQ, ETriggerEvent::Started, this, &APlayerBase::PlayerAttackQ);
	
	}
}


// 기본 이동 조작 인풋 관련 함수 
FVector2D APlayerBase::GetMoveSpeed() const
{
	FVector Maked_Vector = FVector(IA_Move_Forward_Action_Value, IA_Move_Right_Action_Value, 0.f);
	bool bCondition = FMath::Abs(0.586) < FMath::Abs(Maked_Vector.Length());
	
	FVector Result = (bCondition ? Maked_Vector.GetClampedToSize(0.287, 0.332) : Maked_Vector);
	return FVector2D(Result.X, Result.Y);
	
}

void APlayerBase::MoveToFloor()
{
	InitialStepHeight = GetCharacterMovement()->MaxStepHeight;
	GetCharacterMovement()->MaxStepHeight = LargeStepHeight;

	FFindFloorResult Floor;

	
	FVector destlocation;
	FVector capsulelocation = GetCapsuleComponent()->GetComponentLocation();

	GetCharacterMovement()->FindFloor(capsulelocation, Floor, false, nullptr);

	if (Floor.bBlockingHit)
	{
		const float Z = Floor.FloorDist;   // BP의 Floor Result Floor Dist
		const FVector Out(0.f, 0.f, Z);    // BP의 Make Vector(Z=FloorDist)
		destlocation = capsulelocation + Out;
		this->TeleportTo(destlocation, GetActorRotation());
		GetCharacterMovement()->MaxStepHeight = InitialStepHeight;
	}
	
}



void APlayerBase::HandleLookUpInput(const struct FInputActionValue& value)
{
	AddControllerPitchInput(value.Get<float>());
}

void APlayerBase::HandleRightInput(float value)
{
	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
	const FVector  Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Right, value);
}

void APlayerBase::HandleForwardInput(float value)
{
	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
	const FVector  Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Forward, value);
}

void APlayerBase::HandleTurnInput(const struct FInputActionValue& value)
{
	AddControllerYawInput(value.Get<float>());
}

void APlayerBase::MoveForward_Triggered(const FInputActionInstance& Instance)
{
	// action value
	// FInputActionInstance::GetValue()

	// triggered_time
	// Instance.GetTriggeredTime()

	IA_Move_Forward_Action_Value = Instance.GetValue().Get<float>();
	HandleForwardInput(GetMoveSpeed().X);
	
	
}

void APlayerBase::MoveRight_Triggered(const FInputActionInstance& Instance)
{
	IA_Move_Right_Action_Value = Instance.GetValue().Get<float>();
	HandleRightInput(GetMoveSpeed().Y);
	
}

void APlayerBase::JogOverrideAction_Triggered(
	const FInputActionInstance& Instance)
{
	GetCharacterMovement()->MaxWalkSpeed = 427.f;

}

void APlayerBase::JogOverrideAction_Finished(
	const FInputActionInstance& Instance)
{
	GetCharacterMovement()->MaxWalkSpeed =147.f;
	
}



// 무기 소환
void APlayerBase::EnterCombatState()
{
	if (CurrentState == EPlayerStateSimple::Combat) return;
	
	if (SwordInstance || !SwordClass) return;
	
	SwordInstance = GetWorld()->SpawnActor<AWeaponBase>(SwordClass, GetActorTransform());

	if (SwordInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn"));
	}
	if (!SwordInstance) return;

	// 손 소켓에 부착
	USkeletalMeshComponent* CharMesh = GetMesh();
	if (CharMesh)
	{
		SwordInstance->AttachToComponent(
			CharMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SwordSocketName
		);
	}

	CurrentState = EPlayerStateSimple::Combat;
	
}

void APlayerBase::ExitCombatState()
{
	if (CurrentState == EPlayerStateSimple::Movement) return;
	if (SwordInstance)
	{
		SwordInstance->Destroy();
		SwordInstance = nullptr;
	}

	CurrentState = EPlayerStateSimple::Movement;
}

void APlayerBase::OnToggleCombat_Triggered(const FInputActionInstance& Instance)
{
	if (CurrentState == EPlayerStateSimple::Movement) EnterCombatState();
	else ExitCombatState();
	
}


// 공격 함수
void APlayerBase::PlayerAttackQ()
{
	if (CurrentState == EPlayerStateSimple::Movement)
	{
		return;
	}
		
	PlayAnimMontage(montage_q);
}
