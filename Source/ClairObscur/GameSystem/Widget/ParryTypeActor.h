// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParryTypeActor.generated.h"

class UWidgetComponent;

UCLASS()
class CLAIROBSCUR_API AParryTypeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AParryTypeActor();
	
	UFUNCTION(BlueprintCallable)
	void Init(FString DodgeType, const FLinearColor& InColor, float InLifeTime = 1.0f);

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* WidgetComp;

	UPROPERTY()
	class UDamageUIWidget* WidgetInstance = nullptr;

	FString DodgeText = TEXT("PARRIED");
	FLinearColor Color = FLinearColor::White;
	float Life = 1.0f;

	UPROPERTY(EditAnywhere, Category="DamageNumber")
	float RiseSpeed = 90.f;

	UPROPERTY(EditAnywhere, Category="DamageNumber")
	float FadeStart = 0.8f; // Life가 이 값 이하로 내려가면
	
};
