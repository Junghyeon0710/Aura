// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocation()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;//���������� �̴Ͼ����ŭ ����
	
	//���� �߽����� ȸ�� Z�� �������� ȸ�� (����ȭ�ؼ���)
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SpawnLocation;
	for (int32 i = 0; i < NumMinions; i++)
	{
		//���� 45�� ���� �������� Z������ DeltaSpread *i��ŭ ȸ��
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		//���� ������ġ �������� ����
		const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocation.Add(ChosenSpawnLocation);


	}
	return SpawnLocation;
}
