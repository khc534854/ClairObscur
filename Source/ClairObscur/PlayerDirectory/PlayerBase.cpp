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
#include "GameplayTagContainer.h"
#include "PlayerGrappleActor.h"
#include "Components/SplineComponent.h"
#include "Enemy/Enemy.h"
#include "Engine/DamageEvents.h"
#include "GameSystem/Widget/DamageNumberActor.h"
#include "GameSystem/Widget/WidgetComponent/DamagePopupComponent.h"
#include "Kismet/GameplayStatics.h"
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
	springArmComp->bEnableCameraLag = true;

	// fsm
	fsm = CreateDefaultSubobject<UPlayerFSM>(TEXT("FSM"));

	
	// skillComp
	skillComp = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComp"));

	// damageuicomp
	damageComp = CreateDefaultSubobject<UDamagePopupComponent>(TEXT("DamagePopupComp"));

	// tag 붙이기
	Tags.Append({"BattlePossible", "Player"});

	springArmComp->bUsePawnControlRotation = true;
	
	
	//이단 점프
	JumpMaxCount = 2;
	GetCharacterMovement()->JumpZVelocity = 600.f; // 점프 세기
	GetCharacterMovement()->AirControl    = 2.f;  // 공중 방향 전환

	//그래플링
	GrappleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GrappleRoot"));
	GrappleRoot->SetupAttachment(GetMesh());

	MoveSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoveSpline"));
	MoveSpline->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	if (fsm){fsm->warmup();}
	
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
	// 이동속도 초기화
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	// 체력, AP 초기화
	currentHP = maxHP;
	currentAP = maxAP;
	
	
	
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	// 이거 해줘야 다음에 안움직임.
	direction = FVector::ZeroVector;


	// 그래플링
	if (!bGrappling || TotalLen <= 0.f) return;

	CurDist = FMath::Min(CurDist + Speed * DeltaTime, TotalLen);
	UE_LOG(LogTemp, Warning, TEXT("[Grapple] CurDist=%.1f / %.1f"), CurDist, TotalLen);

	const FVector P = MoveSpline->GetLocationAtDistanceAlongSpline(CurDist, ESplineCoordinateSpace::World);
	FVector Delta = P - GetActorLocation();

	FHitResult Hit;
	GetCharacterMovement()->SafeMoveUpdatedComponent(Delta, GetActorRotation(), true, Hit);

	UE_LOG(LogTemp, Warning, TEXT("[Grapple] Move=OK  BlockHit=%s"),
		Hit.bBlockingHit ? TEXT("YES") : TEXT("NO"));

	if (VFX)
	{
		const FVector StartWS = GrappleRoot ? GrappleRoot->GetComponentLocation() : GetActorLocation();
		VFX->UpdateEndpoints(StartWS, AnchorWS);
	}

	if (TotalLen - CurDist <= StopThreshold)
		EndGrapple(true);

	
}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (input)
	{
		input->BindAction(ia_move, ETriggerEvent::Triggered, this, &APlayerBase::MoveInput);
		input->BindAction(ia_run, ETriggerEvent::Started, this, &APlayerBase::RunInput);
		input->BindAction(ia_run, ETriggerEvent::Completed, this, &APlayerBase::RunInput);
		input->BindAction(ia_turn, ETriggerEvent::Triggered, this, &APlayerBase::TurnInput);
		input->BindAction(ia_lookup, ETriggerEvent::Triggered, this, &APlayerBase::LookupInput);
		input->BindAction(ia_jump, ETriggerEvent::Started, this, &APlayerBase::JumpInput);
		input->BindAction(ia_grappling, ETriggerEvent::Started, this, &APlayerBase::OnGrapplePressed);
		input->BindAction(ia_grappling, ETriggerEvent::Completed, this, &APlayerBase::OnGrappleReleased);
	}
}

void APlayerBase::RunInput(const struct FInputActionValue& value)
{
	bool isPressed = value.Get<bool>();
	if (isPressed)
	{
		GetCharacterMovement()-> MaxWalkSpeed=runSpeed;
	}
	else
	{
		GetCharacterMovement()-> MaxWalkSpeed=walkSpeed;
	}
}

void APlayerBase::MoveInput(const struct FInputActionValue& value)
{
	FVector2d v = value.Get<FVector2d>();

	direction.X = v.X;
	direction.Y = v.Y;
}


void APlayerBase::TurnInput(const struct FInputActionValue& value)
{
	
	float v = value.Get<float>();
	AddControllerYawInput(v);
}

void APlayerBase::LookupInput(const struct FInputActionValue& value)
{
	float v = value.Get<float>();
	AddControllerPitchInput(v);
}

void APlayerBase::JumpInput(const struct FInputActionValue& value)
{
	Jump();
}

// 그래플링
void APlayerBase::GetView(FVector& L, FVector& D) const
{
    const APlayerController* PC = Cast<APlayerController>(GetController());
    FRotator R; if (PC) PC->GetPlayerViewPoint(L,R); else { L=GetActorLocation(); R=GetActorRotation(); }
    D = R.Vector();
}

bool APlayerBase::TraceAnchor(FVector& OutHit) const
{
    FVector L,D; GetView(L,D);
    FHitResult H;
	FVector End = L + D * MaxDistance;

	bool bHit = GetWorld()->LineTraceSingleByChannel(H, L, End, ECC_Visibility);
	DrawDebugLine(GetWorld(), L, End,
		bHit ? FColor::Green : FColor::Red, // 맞으면 초록, 아니면 빨강
		false, 1.0f, 0, 2.0f);
	
	if (bHit)
	{
		OutHit = H.ImpactPoint + H.ImpactNormal * 30.f;
		return true;
		
	}
    return false;
}

void APlayerBase::BuildMoveSpline(const FVector& A, const FVector& B, bool UseArc)
{
    MoveSpline->ClearSplinePoints(false);
    MoveSpline->AddSplinePoint(A, ESplineCoordinateSpace::World, false);
    if (UseArc)
    {
        const FVector Mid=0.5f*(A+B);
        const FVector Dir=(B-A).GetSafeNormal();
        const FVector Side=FVector::CrossProduct(Dir, FVector::UpVector).GetSafeNormal();
        const float Dist=FVector::Distance(A,B);
        const float C=FMath::Clamp(Dist*ArcScale,200.f,1200.f);
        MoveSpline->AddSplinePoint(Mid+Side*C, ESplineCoordinateSpace::World, false);
    }
    MoveSpline->AddSplinePoint(B, ESplineCoordinateSpace::World, false);
    for (int32 i=0, n=MoveSpline->GetNumberOfSplinePoints(); i<n; ++i)
        MoveSpline->SetSplinePointType(i, ESplinePointType::Curve, false);
    MoveSpline->UpdateSpline();
    TotalLen = MoveSpline->GetSplineLength();
	UE_LOG(LogTemp, Warning, TEXT("[Grapple] TotalLen=%.1f"), TotalLen);
}

void APlayerBase::BeginGrapple()
{
    bGrappling = true; CurDist = 0.f;
	GetMesh()->GetAnimInstance()->Montage_Play(GrappleMontage);

	UE_LOG(LogTemp, Warning, TEXT("[Grapple] BeginGrapple"));
    if (auto* M=GetCharacterMovement()){
    	M->SetMovementMode(MOVE_Flying);
    	M->GravityScale=0.f; M->Velocity=FVector::ZeroVector;
    	UE_LOG(LogTemp, Warning, TEXT("[Grapple] Mode=Flying, Gravity=0"));
    }

    // VFX(BP_SPLINE) 스폰 + Init
    if (SplineBPClass && !VFX)
    {
        VFX = GetWorld()->SpawnActorDeferred<APlayerGrappleActor>(SplineBPClass, FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        if (VFX)
        {
            const FVector StartWS = GrappleRoot? GrappleRoot->GetComponentLocation() : GetActorLocation();
            VFX->Init(StartWS, AnchorWS, bArc, ArcScale);
            UGameplayStatics::FinishSpawningActor(VFX, FTransform::Identity);
        }
    }
}

void APlayerBase::EndGrapple(bool KeepMomentum)
{
    if (auto* M=GetCharacterMovement()){ M->GravityScale=1.f; M->SetMovementMode(MOVE_Falling); }

    if (KeepMomentum && TotalLen>0.f)
    {
        const FVector Dir = MoveSpline->GetDirectionAtDistanceAlongSpline(TotalLen, ESplineCoordinateSpace::World);
        LaunchCharacter(Dir*600.f, true, true);
    }

    if (VFX){ VFX->Shutdown(); VFX->Destroy(); VFX=nullptr; }
    CurDist=0.f; TotalLen=0.f;
}

void APlayerBase::OnGrapplePressed()
{

	UE_LOG(LogTemp, Warning, TEXT("ongrapple"));
    FVector Hit; if (!TraceAnchor(Hit)) return;
    AnchorWS = Hit;
    const FVector StartWS = GrappleRoot? GrappleRoot->GetComponentLocation() : GetActorLocation();
    BuildMoveSpline(StartWS, AnchorWS, bArc);
    BeginGrapple();
}

void APlayerBase::OnGrappleReleased()
{
    if (bGrappling) EndGrapple(false);
}



// 무기 소환
void APlayerBase::SpawnWeapon()
{
	if (WeaponInstance || !WeaponClass) return;
	WeaponInstance = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetActorTransform());
	
	if (!WeaponInstance) return;

	// 손 소켓에 부착
	USkeletalMeshComponent* CharMesh = GetMesh();
	if (CharMesh)
	{
		WeaponInstance->AttachToComponent(
			CharMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SwordSocketName
		);
	}

	bHasWeapon = true;
}

void APlayerBase::DestroyWeapon()
{
	if (WeaponInstance)
	{
		WeaponInstance->Destroy();
		WeaponInstance = nullptr;
	}

	bHasWeapon = false;
}

void APlayerBase::OnToggleWeapon_Triggered(const FInputActionInstance& Instance)
{
	if (!bHasWeapon) SpawnWeapon();
	else DestroyWeapon();
}




// 플레이어 HP, AP get ,set 
void APlayerBase::setplayerHP(int32 hitdamage, AActor* DamageCauser)
{
	currentHP -= hitdamage;
	currentHP = FMath::Clamp(currentHP, 0,maxHP);

	// change UI
	OnHPChanged.Broadcast(currentHP, maxHP, this);
	
	if (!DamageCauser)
	{
		// WIDGET UI 띄우기
		AController* InstigatorCtrl = nullptr;
		if (APawn* P = Cast<APawn>(DamageCauser)) InstigatorCtrl = P->GetController();
		else if (DamageCauser) InstigatorCtrl = DamageCauser->GetInstigatorController();
	
		TakeDamage((float)hitdamage, FDamageEvent(), InstigatorCtrl, DamageCauser);
	}
}


float APlayerBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
							  AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


int32 APlayerBase::getplayerHP() const
{
	return currentHP;
}

void APlayerBase::setplayerAP(int32 useAP)
{
	currentAP -= useAP;
	currentAP = FMath::Clamp(currentAP, 0,9);
	OnUseAPDelegate.Broadcast(currentAP, this);
}

int32 APlayerBase::getplayerAP() const
{
	return currentAP;
}

void APlayerBase::OnAttackHit()
{
	// "공격이 적중했다!"는 사실을 자신을 포함하여 외부에 방송합니다.
	OnAttackHitDelegate.Broadcast(this);
	
}



