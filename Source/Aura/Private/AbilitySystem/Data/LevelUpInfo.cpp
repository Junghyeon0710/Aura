// Copyright Druid Mechanics


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXp(int32 Xp)
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		//LevelUpInformation[1] = Level 1 Information (0레벨은 아무것도 없음)
		//LevelUpInformation[2] = Level 1 Information (1레벨을 가르킴)
		if (LevelUpInformation.Num() -1 <= Level) return Level;

		if (Xp >= LevelUpInformation[Level].LevelupRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
