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
	int32 LevelupRequirement = 0; //���� �䱸����

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1; //���� �� �Ӽ� ����Ʈ

	UPROPERTY(EditDefaultsOnly)
	int32 SpellpointAward = 1; //���� �� �Ӽ� ����Ʈ
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
