// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BattleFieldComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UBattleFieldComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBattleFieldComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// class UStaticMeshComponent* BaseComp;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* PlayerPos1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* PlayerPos2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* EnemyPos;
};
