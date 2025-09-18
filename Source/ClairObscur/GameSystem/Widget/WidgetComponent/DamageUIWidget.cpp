// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageUIWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UDamageUIWidget::SetDamageAndColor(int32 InDamage,
                                        const FLinearColor& InColor)
{
	
	if (Txt_Damage)
	{
		Txt_Damage->SetText(FText::AsNumber(InDamage));
		Txt_Damage->SetColorAndOpacity(InColor);
	}
	
	
}

void UDamageUIWidget::PlayDamageAnim(float Duration, bool bCritical)
{
	UWidgetAnimation* Anim = TextSizeControl;
	if (!Anim) return;

	// 원하는 표기 지속시간(Duration)에 맞춰 속도 보정 (애니 길이가 1.0s라고 가정하면 1/Duration)
	float PlaybackSpeed = 1.f;
#if ENGINE_MAJOR_VERSION >= 5
	const float AnimLen = Anim->GetEndTime() - Anim->GetStartTime(); 
	if (AnimLen > KINDA_SMALL_NUMBER && Duration > KINDA_SMALL_NUMBER)
		PlaybackSpeed = AnimLen / Duration;
#endif
	PlayAnimation(Anim, 0.f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
	
}


void UDamageUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	

	
}

void UDamageUIWidget::SetPopupColor(FString DodgeType,
	const FLinearColor& InColor)
{
	if (Txt_Damage)
	{
		Txt_Damage->SetText(FText::FromString(DodgeType));
		Txt_Damage->SetColorAndOpacity(InColor);
	}
}
