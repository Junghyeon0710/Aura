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
		//0도 부터 처음 SpawnSpread값까지 구함
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		//적의 스폰위치 랜덤으로 생성
		 FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		
		//언덕일수도 있으니 검사해봄
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			ChosenSpawnLocation + FVector(0.f, 0.f, 400.f),
			ChosenSpawnLocation - FVector(0.f, 0.f, 400.f),
			ECollisionChannel::ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactNormal;
		}

		SpawnLocation.Add(ChosenSpawnLocation);


	}
	return SpawnLocation;
}
