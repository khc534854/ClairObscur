// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerDirectory/PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterComponent/SkillComponent.h"
#include "PlayerDirectory/PlayerFSM.h"
#include "PlayerDirectory/WeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default value
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 회전 설정 off
	bUseControllerRotationYaw = false;
	
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
	springArmComp->SetRelativeLocation(FVector(0.000000,0.000000,57.192264));
	springArmComp->SetRelativeRotation(FRotator(0.000000,90.000000,0.000000));
	springArmComp->TargetArmLength= 200.f;
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->bInheritPitch= true;
	springArmComp->bInheritYaw= true;
	springArmComp->bInheritRoll= true;
	springArmComp->bInheritPitch= true;
	springArmComp->bEnableCameraLag = true;

	// fsm
	fsm = CreateDefaultSubobject<UPlayerFSM>(TEXT("FSM"));

	// skillComp
	skillComp = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComp"));
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
		input->BindAction(IA_SpwanWeapon, ETriggerEvent::Started, this, &APlayerBase::OnToggleWeapon_Triggered);
		input->BindAction(IA_AttackQ, ETriggerEvent::Started, this, &APlayerBase::PlayerAttackQ);
		input->BindAction(IA_CustomJump, ETriggerEvent::Started, this, &APlayerBase::PlayerJump);
	}
}

bool APlayerBase::IsFreeControl() const
{
	return fsm && fsm->GetControlMode() == EControlMode::Free;
}


// 기본 이동 조작 인풋 관련 함수 
FVector2D APlayerBase::GetMoveSpeed() const
{
	FVector Maked_Vector = FVector(IA_Move_Forward_Action_Value, IA_Move_Right_Action_Value, 0.f);
	bool bCondition =  FMath::Abs(Maked_Vector.Length()) < FMath::Abs(0.586)  ;
	
	FVector Result =
		(bCondition ? UKismetMathLibrary::ClampVectorSize(Maked_Vector, 0.287, 0.332) : Maked_Vector);
	return FVector2D(Result.X, Result.Y);

	
}

void APlayerBase::MoveToFloor()
{
	if (!IsFreeControl()) return;
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

// 카메라시점 이동
void APlayerBase::HandleLookUpInput(const struct FInputActionValue& value)
{
	if (!IsFreeControl()) return;
	AddControllerPitchInput(value.Get<float>());
}

void APlayerBase::HandleTurnInput(const struct FInputActionValue& value)
{
	if (!IsFreeControl()) return;
	AddControllerYawInput(value.Get<float>());
}


// 위아래 이동
void APlayerBase::HandleForwardInput(float value)
{
	if (!IsFreeControl()) return;
	FRotator YawRot = UKismetMathLibrary::MakeRotator(0,0,GetControlRotation().Yaw);
	const FVector Forward = UKismetMathLibrary::GetForwardVector(YawRot);
	AddMovementInput(Forward, value);
}

void APlayerBase::MoveForward_Triggered(const FInputActionInstance& Instance)
{
	if (!IsFreeControl()) return; 
	IA_Move_Forward_Action_Value = Instance.GetValue().Get<float>();
	HandleForwardInput(GetMoveSpeed().X);
}


// 좌우 이동
void APlayerBase::HandleRightInput(float value)
{
	if (!IsFreeControl()) return;
	FRotator YawRot = UKismetMathLibrary::MakeRotator(0,0,GetControlRotation().Yaw);
	const FVector Right = UKismetMathLibrary::GetRightVector(YawRot);
	AddMovementInput(Right, value);
}

void APlayerBase::MoveRight_Triggered(const FInputActionInstance& Instance)
{
	if (!IsFreeControl()) return; 
	IA_Move_Right_Action_Value = Instance.GetValue().Get<float>();
	HandleRightInput(GetMoveSpeed().Y);
}

	
// 뛰기
void APlayerBase::JogOverrideAction_Triggered(
	const FInputActionInstance& Instance)
{
	if (!IsFreeControl()) return; 
	GetCharacterMovement()->MaxWalkSpeed = 427.f;

}

void APlayerBase::JogOverrideAction_Finished(
	const FInputActionInstance& Instance)
{
	if (!IsFreeControl()) return; 
	GetCharacterMovement()->MaxWalkSpeed =147.f;
}

void APlayerBase::PlayerJump()
{
	if (!IsFreeControl()) return; 
	Jump();
}

// 무기 소환
void APlayerBase::SpawnWeapon()
{
	if (SwordInstance || !SwordClass) return;
	SwordInstance = GetWorld()->SpawnActor<AWeaponBase>(SwordClass, GetActorTransform());
	
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

	bHasWeapon = true;
}

void APlayerBase::DestroyWeapon()
{
	if (SwordInstance)
	{
		SwordInstance->Destroy();
		SwordInstance = nullptr;
	}

	bHasWeapon = false;
}

void APlayerBase::OnToggleWeapon_Triggered(const FInputActionInstance& Instance)
{
	if (!bHasWeapon) SpawnWeapon();
	else DestroyWeapon();
}



// 공격몽타주재생
void APlayerBase::PlayerAttackQ()
{
	if (!bHasWeapon) {return;}
	PlayAnimMontage(montage_q);
}


