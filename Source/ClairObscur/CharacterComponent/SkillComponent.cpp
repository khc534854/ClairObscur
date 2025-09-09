// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"
#include "GameFramework\Character.h"


// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	
}

void USkillComponent::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	OnActionFinished.Broadcast();

	// 다음 몽타주 재생 시 중복 호출을 막기 위해 바인딩을 해제합니다.
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &USkillComponent::OnMontageCompleted);
	}
}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillComponent::ExecuteSkill(int32 SkillIndex)
{
	if (!OwnerCharacter) return;

	if (SkillDataAssets.IsValidIndex(SkillIndex))
	{
		USkillDataAsset* SkillToUse = SkillDataAssets[SkillIndex];
		if (SkillToUse && SkillToUse->SkillMontage)
		{
			UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
																																																																																																																																																																																											
				// 몽타주가 끝나면 OnMontageCompleted 함수를 호출하도록 바인딩합니다.
				AnimInstance->OnMontageEnded.AddDynamic(this, &USkillComponent::OnMontageCompleted);
				// 몽타주를 재생합니다.
				OwnerCharacter->PlayAnimMontage(SkillToUse->SkillMontage);
			}
		}
	}
}
