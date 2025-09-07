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

private:
	// 모든 위젯을 숨기기 위한 헬퍼 함수
	void HideAllWidgets();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnFSMStateChanged(EBattleState NewState);

	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SelectActionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SelectSkillWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TimingWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SelectTargetWidgetClass;


	UPROPERTY()
	class UUserWidget* SelectActionWidget;

	UPROPERTY()
	class UUserWidget* SelectSkillWidget;
	UPROPERTY()
	class UUserWidget* TimingWidget;
	UPROPERTY()
	class UUserWidget* SelectTargetWidget;

	UPROPERTY()
	TArray<UUserWidget*> AllWidgets;
};
