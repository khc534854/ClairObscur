// Fill out your copyright notice in the Description page of Project Settings.


#include "ParryTypeActor.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetComponent/DamageUIWidget.h"


// Sets default values
AParryTypeActor::AParryTypeActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	RootComponent = WidgetComp;
	//WidgetComp->SetRelativeScale3D(FVector(1));
	WidgetComp->SetWidgetSpace(EWidgetSpace::World);
	WidgetComp->SetDrawAtDesiredSize(true);
	//WidgetComp->SetDrawSize(FVector2D(512,256));
	WidgetComp->SetTwoSided(true);
	WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
	
}

void AParryTypeActor::Init(FString DodgeType, const FLinearColor& InColor,
	float InLifeTime)
{
	DodgeText = DodgeType;
	Color = InColor;
	Life   = InLifeTime;  
	if (WidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("widgetinstance not null"));
		WidgetInstance->SetPopupColor(DodgeText, Color);
		WidgetInstance->SetRenderOpacity(1.0f);
		WidgetInstance->PlayDamageAnim(Life, false);
	}
	
}

// Called when the game starts or when spawned
void AParryTypeActor::BeginPlay()
{
	Super::BeginPlay();

	if (UUserWidget* UW = WidgetComp->GetUserWidgetObject())
	{
		WidgetInstance = Cast<UDamageUIWidget>(UW);
		if (WidgetInstance)
		{
			WidgetInstance->SetPopupColor(DodgeText, Color);
			WidgetInstance->SetRenderOpacity(1.0f);
			WidgetInstance->PlayDamageAnim(Life, false);
		}
	}
	
	SetLifeSpan(Life);
	
}

// Called every frame
void AParryTypeActor::Tick(float DeltaTime)
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

