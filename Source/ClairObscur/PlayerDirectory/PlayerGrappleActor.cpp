// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGrappleActor.h"

#include "NiagaraComponent.h"


// Sets default values
APlayerGrappleActor::APlayerGrappleActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerGrappleActor::BeginPlay()
{
	Super::BeginPlay();
    Cache();
	
}

// Called every frame
void APlayerGrappleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APlayerGrappleActor::OnConstruction(const FTransform&){ Cache(); }

void APlayerGrappleActor::Cache()
{
    if (!Spline)  Spline  = FindComponentByClass<USplineComponent>();
    if (!Niagara) Niagara = FindComponentByClass<UNiagaraComponent>();
}

void APlayerGrappleActor::BuildSpline(const FVector& A, const FVector& B, bool bUseArc, float ArcScale)
{
    if (!Spline) { Cache(); }
    if (!Spline) return;

    Spline->ClearSplinePoints(false);
    Spline->AddSplinePoint(A, ESplineCoordinateSpace::World, false);

    if (bUseArc)
    {
        const FVector Mid = 0.5f*(A+B);
        const FVector Dir = (B-A).GetSafeNormal();
        const FVector Side= FVector::CrossProduct(Dir, FVector::UpVector).GetSafeNormal();
        const float Dist = FVector::Distance(A,B);
        const float CurveAmt = FMath::Clamp(Dist*ArcScale, 200.f, 1200.f);
        Spline->AddSplinePoint(Mid + Side*CurveAmt, ESplineCoordinateSpace::World, false);
    }

    Spline->AddSplinePoint(B, ESplineCoordinateSpace::World, false);
    for (int32 i=0, n=Spline->GetNumberOfSplinePoints(); i<n; ++i)
        Spline->SetSplinePointType(i, ESplinePointType::Curve, false);
    Spline->UpdateSpline();
}

void APlayerGrappleActor::Init(const FVector& StartWS, const FVector& EndWS, bool bUseArc, float ArcScale)
{
    BuildSpline(StartWS, EndWS, bUseArc, ArcScale);
    if (Niagara && !Niagara->IsActive()) Niagara->Activate(true); // NS 켜기(한 줄)
}

void APlayerGrappleActor::UpdateEndpoints(const FVector& StartWS, const FVector& EndWS)
{
    if (!Spline){ Cache(); }
    if (Spline)
    {
        const int32 Last = Spline->GetNumberOfSplinePoints()-1;
        if (Last >= 1)
        {
            Spline->SetLocationAtSplinePoint(0,    StartWS, ESplineCoordinateSpace::World, false);
            Spline->SetLocationAtSplinePoint(Last, EndWS,   ESplineCoordinateSpace::World, false);
            Spline->UpdateSpline();
        }
    }
    if (Niagara && !Niagara->IsActive()) Niagara->Activate(true); // 필요 시 자동 재활성
}

void APlayerGrappleActor::Shutdown()
{
    if (Niagara && Niagara->IsActive()) Niagara->DeactivateImmediate();
}
