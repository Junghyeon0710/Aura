// Copyright Druid Mechanics


#include "AI/BTService_FindNearestplayer.h"
#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestplayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwingPawn = AIOwner->GetPawn();

	const FName TargetTag = OwingPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> ActorsWithTag;
	/** �±� �������ִ� ���͸� ��� ã�´�.*/
	UGameplayStatics::GetAllActorsWithTag(OwingPawn, TargetTag, ActorsWithTag);

	//float�� �ִ�
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* Actor : ActorsWithTag)
	{
		if (IsValid(Actor) && IsValid(OwingPawn))
		{
			//������ �±� �������ִ� �÷��̾� ���� �Ÿ�
			const float Distance = OwingPawn->GetDistanceTo(Actor);
			if(Distance < ClosestDistance) //�Ÿ��� �� ������
			{
				//�Ÿ� ����� �Ÿ� �ٲ���
				ClosestDistance = Distance;
				//���� ����� ����
				ClosestActor = Actor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
