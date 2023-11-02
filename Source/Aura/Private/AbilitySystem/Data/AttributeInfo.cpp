// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttriuteTag, bool bLogNotFound) const
{
    //받은값이랑 데이터 에셋에 있는 태그를 비교
    for (const FAuraAttributeInfo& Info : AttributeInformation)
    {
        // 찾았으면
        if (Info.AttributeTag.MatchesTagExact(AttriuteTag))
        {
            //맞는 태그가 정보가 있는 배열 인덱스를 반환
           return Info;
        }
    }

    //없으면 출력
    if (bLogNotFound)
    {
        UE_LOG(LogTemp,Error,TEXT("Can't find Info for AttributeTag [%s] on AttriubteInfo[%s"),*AttriuteTag.ToString(),*GetNameSafe(this));

    }

    return FAuraAttributeInfo();
}
