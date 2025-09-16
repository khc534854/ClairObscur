// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamagePopupComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UDamagePopupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDamagePopupComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction*
	                           ThisTickFunction) override;

	UFUNCTION() // AnyDamage 콜백
	void OnAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
					 class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION() // PointDamage 콜백
	void OnPointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy,
					   FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName,
					   FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	

	FVector ComputeImpactFacingAttacker(const AActor* Attacker ) const;

	
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	TSubclassOf<class ADamageNumberActor> DamageNumberClass;

	void SpawnDamageNumberAt(const FVector& WorldLoc, int32 Amount, bool bVictimIsPlayer, bool bCritical=false);
	
};
