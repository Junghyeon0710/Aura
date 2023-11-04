// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttriuteTag, bool bLogNotFound) const
{
    //�������̶� ������ ���¿� �ִ� �±׸� ��
    for (const FAuraAttributeInfo& Info : AttributeInformation)
    {
        // ã������
        if (Info.AttributeTag.MatchesTagExact(AttriuteTag))
        {
            //�´� �±װ� ������ �ִ� �迭 �ε����� ��ȯ
           return Info;
        }
    }

    //������ ���
    if (bLogNotFound)
    {
        UE_LOG(LogTemp,Error,TEXT("Can't find Info for AttributeTag [%s] on AttriubteInfo[%s"),*AttriuteTag.ToString(),*GetNameSafe(this));

    }

    return FAuraAttributeInfo();
}