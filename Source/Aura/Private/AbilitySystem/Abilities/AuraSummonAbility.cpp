// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocation()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;//스폰각도를 미니언수만큼 나눔
	
	//축을 중심으로 회전 Z축 기준으로 회전 (정규화해서줌)
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SpawnLocation;
	for (int32 i = 0; i < NumMinions; i++)
	{
		//적의 45도 왼쪽 각도에서 Z축으로 DeltaSpread *i만큼 회전
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		//적의 스폰위치 랜덤으로 생성
		const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocation.Add(ChosenSpawnLocation);


	}
	return SpawnLocation;
}
