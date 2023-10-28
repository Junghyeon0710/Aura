// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor =Cast<IEnemyInterface>(CursorHit.GetActor());

	/** Ŀ�� ���� Ʈ���̽�
	* A.������ ���Ͱ� �� && ���� ���͵� �� (���� �Ȱ���Ű�� ����)
	*   -�ƹ��͵� ��������
	* B. LastActor �� && ���� ���� ���� ���� (���� ����Ű����)
	*	-���̶���Ʈ thisActor
	* C. LastActor ���� �ְ� && ThisActor �� (������ ���콺�� �ű�)
	*  - LastActor ���̶���Ʈ ����
	* D. �Ѵ� ���� �ְ�, ���� �ٸ� ���϶� (�ٸ� ������ �ű�)
	*  - Last ����Ÿ��Ʈ ���� , thisActor ���̶�Ʈ
	* E. �Ѵ� ���� �ְ�, ���� ���� �� (���� ����Ű�� �ִٴ� ��)
	*  - Return
	*/

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			//Case B
			ThisActor->HighlightActor();
		}
		else
		{
			//Case A = �Ѵ� �� , �ƹ��͵� ��������
		}
	}
	else // LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			//case C
			LastActor->UnHighlightActor();
		}
		else // �Ѵ� ���� ����
		{
			if (LastActor != ThisActor)
			{
				//Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//Case E , �ƹ��͵� ���� ����
			}
		}
	}

}
void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default; //���콺Ŀ�� ���

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControllerdPawn = GetPawn<APawn>())
	{
		ControllerdPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); //�����Ϳ��� Y
		ControllerdPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}


