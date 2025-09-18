// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagePopupComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameSystem/Widget/DamageNumberActor.h"
#include "GameSystem/Widget/ParryTypeActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerDirectory/PlayerBase.h"


// Sets default values for this component's properties
UDamagePopupComponent::UDamagePopupComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDamagePopupComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UDamagePopupComponent::OnAnyDamage);
		Owner->OnTakePointDamage.AddDynamic(this, &UDamagePopupComponent::OnPointDamage);
	}
	
}


// Called every frame
void UDamagePopupComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction*
                                          ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UDamagePopupComponent::OnAnyDamage(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy,
	AActor* DamageCauser)
{

	
	if (!DamageNumberClass || Damage <= 0.f) return;

	const FVector Impact = ComputeImpactFacingAttacker(DamageCauser);

	bool bVictimIsPlayer = false;
	if (const APawn* Pawn = Cast<APawn>(DamagedActor)) bVictimIsPlayer = Pawn->IsPlayerControlled();

	
	SpawnDamageNumberAt(Impact, FMath::RoundToInt(Damage), bVictimIsPlayer, /*bCritical*/false);

}

void UDamagePopupComponent::OnPointDamage(AActor* DamagedActor, float Damage,
	class AController* InstigatedBy, FVector HitLocation,
	class UPrimitiveComponent* HitComponent, FName BoneName,
	FVector ShotFromDirection, const class UDamageType* DamageType,
	AActor* DamageCauser)
{
	if (!DamageNumberClass || Damage <= 0.f) return;

	bool bVictimIsPlayer = false;
	if (const APawn* Pawn = Cast<APawn>(DamagedActor)) bVictimIsPlayer = Pawn->IsPlayerControlled();

	APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector CamLoc; FRotator CamRot;
	PCM->GetCameraViewPoint(CamLoc, CamRot);

	FVector DirFromCam = (CamLoc - HitLocation).GetSafeNormal();
	FVector SpawnLoc = HitLocation + DirFromCam * 20.f; // 카메라 쪽으로 20cm
	
	// 포인트 데미지는 엔진이 위치를 줌 → 그대로 사용
	SpawnDamageNumberAt(SpawnLoc, FMath::RoundToInt(Damage), bVictimIsPlayer, false);

}


FVector UDamagePopupComponent::ComputeImpactFacingAttacker(
	const AActor* Attacker) const
{

	ACharacter* compOwner = Cast<ACharacter>(GetOwner());
	
	const UCapsuleComponent* Capsule = compOwner-> GetCapsuleComponent();
	const float Radius     = Capsule ? Capsule->GetScaledCapsuleRadius()     : 40.f;
	const float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 88.f;

	// 가슴 기준
	FVector Chest = compOwner -> GetActorLocation() + FVector(0,0, HalfHeight * 0.6f);
	if (const USkeletalMeshComponent* SkelMesh = Cast<ACharacter>(GetOwner())-> GetMesh())
	{
		static const FName Spine(TEXT("spine_03"));
		if (SkelMesh->DoesSocketExist(Spine))
			Chest = SkelMesh->GetSocketLocation(Spine);
	}

	// 피격자 → 공격자 방향
	FVector AttLoc = Attacker ? Attacker->GetActorLocation() : Chest + compOwner->GetActorForwardVector()*100.f;
	FVector Dir = (AttLoc - Chest); 
	Dir.Z = 0.f;                  
	if (!Dir.Normalize()) Dir = compOwner->GetActorForwardVector();

	// 공격자 쪽 표면
	FVector Impact = Chest + Dir * (Radius + 2.f);
	Impact.Z += 8.f;
	
	const FVector J2 = 10.f * UKismetMathLibrary::RandomUnitVector();
	Impact += FVector(J2.X, J2.Y, 0.f);
	return Impact;
}



void UDamagePopupComponent::SpawnDamageNumberAt(const FVector& WorldLoc,
	int32 Amount, bool bVictimIsPlayer, bool bCritical)
{
	if (!DamageNumberClass) return;

	// 색 규칙
	const FLinearColor Color_Damage =  FLinearColor(1.0f, 1.f, 1.f, 1.0f) ;
	
	FActorSpawnParameters P; 
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 겹침 방지
	const FVector J = 12.f * UKismetMathLibrary::RandomUnitVector();



	auto* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector CamLoc; FRotator CamRot;
	PCM->GetCameraViewPoint(CamLoc, CamRot);

	const FVector CamForward = CamRot.Vector();
	const float ForwardDist = 60.f;   // 카메라 쪽으로 forward

	const FVector Loc = WorldLoc - CamForward * ForwardDist + FVector(J.X, J.Y, 0);
	
	if (ADamageNumberActor* A = GetWorld()->SpawnActor<ADamageNumberActor>(DamageNumberClass, Loc, FRotator::ZeroRotator, P))
	{
		A->Init(Amount, Color_Damage,1.0f);  // duration
	}
}


void UDamagePopupComponent::SpawnDodgeTypeAt(const FVector& WorldLoc,
	FString DodgeType)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnDodgeTypeat Call"));
	if (!ParryTypeClass){ 	UE_LOG(LogTemp, Warning, TEXT("parrytype null Call"));
		return;} 

	bool bDodge = false;
	if (DodgeType == TEXT("PERFECT")){bDodge= true;} 
	
	// 색 규칙
	const FLinearColor Color = bDodge ? FLinearColor(1.0f, 0.72f, 0.47f, 1.0f)
		: FLinearColor(0.68f, 0.85f, 0.90f, 1.0f); 
	
	// 겹침 방지
	const FVector J = 12.f * UKismetMathLibrary::RandomUnitVector();
	//const FVector Loc = WorldLoc + FVector(J.X, J.Y, 0);

	auto* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector CamLoc; FRotator CamRot;
	PCM->GetCameraViewPoint(CamLoc, CamRot);

	const FVector CamForward = CamRot.Vector();
	const float ForwardDist = 60.f;   // 카메라 쪽

	const FVector Loc = WorldLoc - CamForward * ForwardDist +FVector(J.X, J.Y, 0);


	
	if (AParryTypeActor* A = GetWorld()->SpawnActor<AParryTypeActor>(ParryTypeClass, Loc, FRotator::ZeroRotator))
	{
		A->Init(DodgeType, Color,1.0f);  // duration
	}
}





