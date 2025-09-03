// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillDataAsset.generated.h"

UENUM(BlueprintType)
enum class ESkillTargetType : uint8
{
	Self,          // 자신
	SingleEnemy,   // 적 1명
	AllEnemies,    // 모든 적
	SingleAlly,    // 아군 1명
	AllAllies      // 모든 아군
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	BasicAttack,
	ComboAttack,
	Buff,
	Debuff
};


UCLASS()
class CLAIROBSCUR_API USkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText SkillName;

	// 스킬 사용 시 재생할 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	UAnimMontage* SkillMontage;

	// 스킬 소모 코스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	int32 Cost;

	// 스킬의 기본 위력 (최종 데미지는 이 값과 캐릭터 스탯을 조합하여 계산)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float Power;

	// 스킬의 타겟팅 방식
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillTargetType TargetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	int32 ComboCount = 1;
};
