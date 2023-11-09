// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouser.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouser* UTargetDataUnderMouser::CreateTargetDataUnderMouser(UGameplayAbility* OwingAbility)
{
	UTargetDataUnderMouser* MyObj = NewAbilityTask<UTargetDataUnderMouser>(OwingAbility);
	return MyObj;
}

void UTargetDataUnderMouser::Activate()
{
	//현재 어빌리티를 실행하고 있는 액터가 로컬 플레이어에 의해 컨트롤되고 있는지 여부를 확인
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else //서버에서 실행 중이므로 대상 데이터를 수신 대기합니다
	{
		//어빌리티의 스펙 핸들을 얻어옵니다
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		//어빌리티의 활성화 예측 키를 얻어옵니다.
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		//해당 데이터가 복제될 때 함수 호출
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouser::OnTargetDataReplicatedCallback);
		//하지만 너무 늦어서 복제가 안되었다면?
		//해당 타겟 데이터가 복제 되았나 true / false
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			// 대리자를 호출하지 않았다면 이는 아직 서버에 도달하지 않았다는 의미입니다.
			//그래서 우리는 그 데이터가 돌아올 때까지 기다림
			SetWaitingOnRemotePlayerData();
		}
	}

}

//마우스 커서 위치에서 어떤 대상을 선택하고 해당 정보를 다른 클라이언트와 서버로 공유하는 데 사용
void UTargetDataUnderMouser::SendMouseCursorData()
{
	//클라이언트로부터 새로운 예측 키가 수신되면 서버에서 호출
	//새로운 예측 키를 생성하고 클라이언트와 서버 간의 동기화 지점 역할
	//게임의 다중 플레이어 환경에서 무결성 및 동기화를 보장하는 데 중요한 역할
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	//어빌리티에 전달할 대상 데이터를 포함하는 핸들
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	FGameplayTag ApplicationTag;
	//대상 데이터를 서버로 전송하여 다른 클라이언트와 공유하며, 미래 예측 동안 사용할 예측 키 및 어빌리티 스펙 핸들과 함께 데이터를 보냅니다.
	//이 함수는 서버에서만 호출
	/**
	*GetAbilitySpecHandle(): 어빌리티의 스펙(특정 어빌리티의 고유 식별자) 핸들을 가져옵니다. 이 핸들은 어빌리티를 고유하게 식별하고 서버와 클라이언트 간에 통신할 때 어빌리티를 식별하는 데 사용됩니다.
	*GetActivationPredictionKey(): 예측 키를 가져옵니다. 언리얼 어빌리티 시스템에서는 예측을 통해 원활한 네트워크 게임 플레이를 지원합니다. 
	*예측 키는 예측과 관련된 정보를 나타내며, 서버 및 클라이언트 간에 동기화를 유지하는 데 사용됩니다.
	*DataHandle: 타겟 데이터의 핸들이며, 타겟 데이터는 이 코드에서 설정하려는 정보를 포함합니다.
	*ApplicationTag: 어빌리티를 적용할 때 사용하는 게임플레이 태그입니다.
	*AbilitySystemComponent->ScopedPredictionKey: 예측 키의 범위를 나타냅니다. 이 값은 예측 키를 스코프 내에서만 유효하도록 설정합니다.
	*/
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(), 
		GetActivationPredictionKey(),
		DataHandle, 
		ApplicationTag, 
		AbilitySystemComponent->ScopedPredictionKey);

	//능력이 활성화되었는지 확인합니다. 만약 활성화되어 있지 않다면 아래의 브로드캐스트도 실행되지 않습니다.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouser::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// 클라이언트는 사용이 완료된 대상 데이터를 제거하고 능력의 실행을 마무리
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
//클라이언트에서 Activate하면 서버에서도 Activate하는데 만약 값을 보낼러면 RPC를 해야한다.
//근데 여기서 문제가 발생한다 복제된 값이 Activate보다 늦게 서버로 도착하면
//Activate할 때 값을 전달을 못하게 된다.