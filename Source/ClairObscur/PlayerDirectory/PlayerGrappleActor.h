// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "PlayerGrappleActor.generated.h"

class UNiagaraComponent;

UCLASS()
class CLAIROBSCUR_API APlayerGrappleActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerGrappleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;


	UFUNCTION(BlueprintCallable)
	void Init(const FVector& StartWS, const FVector& EndWS, bool bUseArc=false, float ArcScale=0.25f);
	UFUNCTION(BlueprintCallable)
	void UpdateEndpoints(const FVector& StartWS, const FVector& EndWS);
	UFUNCTION(BlueprintCallable)
	void Shutdown();
	
	
private:
	UPROPERTY(Transient)
	USplineComponent*   Spline  = nullptr;
	UPROPERTY(Transient)
	UNiagaraComponent* Niagara = nullptr;

	void Cache();
	void BuildSpline(const FVector& StartWS, const FVector& EndWS, bool bUseArc, float ArcScale);

	
};
