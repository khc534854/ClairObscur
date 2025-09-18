// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "DamageNumberActor.generated.h"

UCLASS()
class CLAIROBSCUR_API ADamageNumberActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADamageNumberActor();

	UFUNCTION(BlueprintCallable)
	void Init(int32 InDamage, const FLinearColor& InColor, float InLifeTime = 1.0f);

	

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

	int32 Damage = 0;
	FLinearColor Color = FLinearColor::White;
	float Life = 1.0f;

	UPROPERTY(EditAnywhere, Category="DamageNumber")
	float RiseSpeed = 90.f;

	UPROPERTY(EditAnywhere, Category="DamageNumber")
	float FadeStart = 0.8f; // Life가 이 값 이하로 내려가면 서서히 투명
	
};
