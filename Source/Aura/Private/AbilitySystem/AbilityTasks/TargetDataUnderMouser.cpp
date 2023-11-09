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
	//���� �����Ƽ�� �����ϰ� �ִ� ���Ͱ� ���� �÷��̾ ���� ��Ʈ�ѵǰ� �ִ��� ���θ� Ȯ��
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else //�������� ���� ���̹Ƿ� ��� �����͸� ���� ����մϴ�
	{
		//�����Ƽ�� ���� �ڵ��� ���ɴϴ�
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		//�����Ƽ�� Ȱ��ȭ ���� Ű�� ���ɴϴ�.
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		//�ش� �����Ͱ� ������ �� �Լ� ȣ��
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouser::OnTargetDataReplicatedCallback);
		//������ �ʹ� �ʾ ������ �ȵǾ��ٸ�?
		//�ش� Ÿ�� �����Ͱ� ���� �Ǿҳ� true / false
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			// �븮�ڸ� ȣ������ �ʾҴٸ� �̴� ���� ������ �������� �ʾҴٴ� �ǹ��Դϴ�.
			//�׷��� �츮�� �� �����Ͱ� ���ƿ� ������ ��ٸ�
			SetWaitingOnRemotePlayerData();
		}
	}

}

//���콺 Ŀ�� ��ġ���� � ����� �����ϰ� �ش� ������ �ٸ� Ŭ���̾�Ʈ�� ������ �����ϴ� �� ���
void UTargetDataUnderMouser::SendMouseCursorData()
{
	//Ŭ���̾�Ʈ�κ��� ���ο� ���� Ű�� ���ŵǸ� �������� ȣ��
	//���ο� ���� Ű�� �����ϰ� Ŭ���̾�Ʈ�� ���� ���� ����ȭ ���� ����
	//������ ���� �÷��̾� ȯ�濡�� ���Ἲ �� ����ȭ�� �����ϴ� �� �߿��� ����
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	//�����Ƽ�� ������ ��� �����͸� �����ϴ� �ڵ�
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	FGameplayTag ApplicationTag;
	//��� �����͸� ������ �����Ͽ� �ٸ� Ŭ���̾�Ʈ�� �����ϸ�, �̷� ���� ���� ����� ���� Ű �� �����Ƽ ���� �ڵ�� �Բ� �����͸� �����ϴ�.
	//�� �Լ��� ���������� ȣ��
	/**
	*GetAbilitySpecHandle(): �����Ƽ�� ����(Ư�� �����Ƽ�� ���� �ĺ���) �ڵ��� �����ɴϴ�. �� �ڵ��� �����Ƽ�� �����ϰ� �ĺ��ϰ� ������ Ŭ���̾�Ʈ ���� ����� �� �����Ƽ�� �ĺ��ϴ� �� ���˴ϴ�.
	*GetActivationPredictionKey(): ���� Ű�� �����ɴϴ�. �𸮾� �����Ƽ �ý��ۿ����� ������ ���� ��Ȱ�� ��Ʈ��ũ ���� �÷��̸� �����մϴ�. 
	*���� Ű�� ������ ���õ� ������ ��Ÿ����, ���� �� Ŭ���̾�Ʈ ���� ����ȭ�� �����ϴ� �� ���˴ϴ�.
	*DataHandle: Ÿ�� �������� �ڵ��̸�, Ÿ�� �����ʹ� �� �ڵ忡�� �����Ϸ��� ������ �����մϴ�.
	*ApplicationTag: �����Ƽ�� ������ �� ����ϴ� �����÷��� �±��Դϴ�.
	*AbilitySystemComponent->ScopedPredictionKey: ���� Ű�� ������ ��Ÿ���ϴ�. �� ���� ���� Ű�� ������ �������� ��ȿ�ϵ��� �����մϴ�.
	*/
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(), 
		GetActivationPredictionKey(),
		DataHandle, 
		ApplicationTag, 
		AbilitySystemComponent->ScopedPredictionKey);

	//�ɷ��� Ȱ��ȭ�Ǿ����� Ȯ���մϴ�. ���� Ȱ��ȭ�Ǿ� ���� �ʴٸ� �Ʒ��� ��ε�ĳ��Ʈ�� ������� �ʽ��ϴ�.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouser::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// Ŭ���̾�Ʈ�� ����� �Ϸ�� ��� �����͸� �����ϰ� �ɷ��� ������ ������
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
//Ŭ���̾�Ʈ���� Activate�ϸ� ���������� Activate�ϴµ� ���� ���� �������� RPC�� �ؾ��Ѵ�.
//�ٵ� ���⼭ ������ �߻��Ѵ� ������ ���� Activate���� �ʰ� ������ �����ϸ�
//Activate�� �� ���� ������ ���ϰ� �ȴ�.