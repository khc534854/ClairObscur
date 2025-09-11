// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CostBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLAIROBSCUR_API UCostBarWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	void SetCost(int32 newCost);
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost1;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost2;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost3;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost4;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost5;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost6;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost7;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost8;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UImage* Cost9;

	TArray<class UImage*> Costs;
};
