// Fill out your copyright notice in the Description page of Project Settings.
#include "DamageNumberActor.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetComponent/DamageUIWidget.h"


// Sets default values
ADamageNumberActor::ADamageNumberActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	RootComponent = WidgetComp;
	WidgetComp->SetWidgetSpace(EWidgetSpace::World);
	WidgetComp->SetDrawAtDesiredSize(true);
	WidgetComp->SetTwoSided(true);
	WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
	
}

void ADamageNumberActor::Init(int32 InDamage, const FLinearColor& InColor,
	float InLifeTime)
{
	Damage = InDamage;
	Color  = InColor;
	Life   = InLifeTime;

	if (WidgetInstance)
	{
		WidgetInstance->SetDamageAndColor(Damage, Color);
		WidgetInstance->SetRenderOpacity(1.0f);
		WidgetInstance->PlayDamageAnim(Life, false);
	}
}

// Called when the game starts or when spawned
void ADamageNumberActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[DamageNumber] BeginPlay. Widget=%s"), *GetNameSafe(WidgetComp->GetUserWidgetObject()));

	
	if (UUserWidget* UW = WidgetComp->GetUserWidgetObject())
	{
		WidgetInstance = Cast<UDamageUIWidget>(UW);
		if (WidgetInstance)
		{
			WidgetInstance->SetDamageAndColor(Damage, Color);
			WidgetInstance->SetRenderOpacity(1.0f);
			WidgetInstance->PlayDamageAnim(Life, false);
		}
	}
	
	SetLifeSpan(Life);
}

// Called every frame
void ADamageNumberActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 1) 위로 상승
	AddActorWorldOffset(FVector(0,0,RiseSpeed * DeltaTime));

	// 2) 카메라 바라보기(Yaw만)
	if (APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		const FVector CamLoc = PCM->GetCameraLocation();
		FRotator Look = (CamLoc - GetActorLocation()).Rotation();
		Look.Pitch = 0.f; Look.Roll = 0.f;
		SetActorRotation(Look);
	}
	
}

