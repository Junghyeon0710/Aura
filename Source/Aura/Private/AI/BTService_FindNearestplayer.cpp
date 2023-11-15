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
	/** 태그 가지고있는 액터를 모두 찾는다.*/
	UGameplayStatics::GetAllActorsWithTag(OwingPawn, TargetTag, ActorsWithTag);

	//float의 최댓값
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* Actor : ActorsWithTag)
	{
		if (IsValid(Actor) && IsValid(OwingPawn))
		{
			//적에서 태그 가지고있는 플레이어 까지 거리
			const float Distance = OwingPawn->GetDistanceTo(Actor);
			if(Distance < ClosestDistance) //거리가 더 가까우면
			{
				//거리 가까운 거리 바꿔줌
				ClosestDistance = Distance;
				//가장 가까운 엑터
				ClosestActor = Actor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
