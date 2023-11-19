// Copyright Druid Mechanics


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXp(int32 Xp)
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		//LevelUpInformation[1] = Level 1 Information (0번쨰를 가르킴)
		//LevelUpInformation[2] = Level 1 Information (1번째를 가르킴)
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
