#pragma once

#pragma once
#include "Engine/DataTable.h"
#include "SkillRow.generated.h"

USTRUCT(BlueprintType)
struct FSkillRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
	FText SkillDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BasicDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> SkillMontage;
	
};
