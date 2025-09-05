// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleUIComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLAIROBSCUR_API UBattleUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBattleUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnFSMStateChanged(EBattleState NewState);

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* SelectActionWidget;
	//TSubclassOf<UUserWidget> SelectionActionwid;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* SelectSkillWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* TimingWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* SelectTargetWidget;
};
