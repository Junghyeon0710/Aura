// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelupRequirement = 0; //레벨 요구사항

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1; //레벨 업 속성 포인트

	UPROPERTY(EditDefaultsOnly)
	int32 SpellpointAward = 1; //스펠 업 속성 포인트
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;

	int32 FindLevelForXp(int32 Xp) const;
};
