// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameSystem/Component/BattleResultDataComponent.h"
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
	// 모든 위젯을 숨기기 위한 헬퍼 함수
	void HideBattleWidgets();
	void HideAllWidgets();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnFSMStateChanged(EBattleState NewState);

	void ShowQTEWidget();

	void UpdateHUD();

	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SelectActionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SelectSkillWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TimingWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SelectTargetWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EndWinWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EndLoseWidgetClass;
	
	UPROPERTY()
	class UUserWidget* SelectActionWidget;

	UPROPERTY()
	class UUserWidget* SelectSkillWidget;
	UPROPERTY()
	class UUserWidget* TimingWidget;
	UPROPERTY()
	class UUserWidget* SelectTargetWidget;
	UPROPERTY()
	class UBattleHUDWidget* BattleHUDWidget;

	UPROPERTY()
	class UBattleEndWidget* BattleWinWidget;
	
	UPROPERTY()
	class UUserWidget* BattleLoseWidget;


	UPROPERTY()
	TArray<UUserWidget*> AllWidgets;


	// Die Widget 업데이트
	UFUNCTION()
	void OnBattleEnded();

	UFUNCTION()
	void OnBattleLoseEnded();

	FBattleResult PendingWinResult;
	bool bHasPendingWinResult = false;

	void ShowWinWidgetIfReady();
};
