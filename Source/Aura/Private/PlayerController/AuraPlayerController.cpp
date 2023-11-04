// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();

}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		//ControlledPawn�� ��ġ�� ������� Spline �󿡼� ���� ����� ��ġ�� ã�� LocationOnSpline�� �����մϴ�. 
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// LocationOnSpline ��ġ���� ���� ����� ������ ������ ã�Ƴ���
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		//�� �������� �̵�
		ControlledPawn->AddMovementInput(Direction);

		//ĳ���� ��ġ���� ���� Spline��ġ ���� �Ÿ�
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();

		//��ġ�� �����ϸ�
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
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

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		/** Ÿ���� ������ Ʈ��*/
		bTargeting = ThisActor ? true : false;
		/** �������� ���� �� ��*/
		bAutoRunning = false;
	}

}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//���ʸ��콺 ��ư �ƴϸ�
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		//�ɷ� ��Ȱ��ȭ
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	//Ÿ�ٿ� ���ٵǸ�
	if (bTargeting)
	{
		//�ɷ� ��Ȱ��ȭ
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
	}
	else // ��ġ�� ���ٵǸ�
	{
		APawn* ControllerPawn = GetPawn();
		//ª�� �������� �� �������� ���ߵ�
		if (FollowTime <= ShortPressThreshold && ControllerPawn)
		{
			/**
			* ������̼� �ý��ۿ��� Ư�� ��ġ�� ��θ� ���������� ã�� �Լ��Դϴ�.
			*�� �Լ��� �־��� ���� ��ġ���� ������ ��ġ������ ��θ� ����ϰ� �� ����� ��� ��ȯ�մϴ�.
			*�������̶�� ���� �Լ��� ��θ� ����� ������ ����Ѵٴ� ���� �ǹ��մϴ�. 
			*���� �� �Լ��� �Ϸ�� ������ ���α׷��� ���� �ڵ�� �������� �ʽ��ϴ�.
			*/
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControllerPawn->GetActorLocation(), CachedDestination))
			{
				//����Ʈ�� ������
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					//���ʶ��� ����Ʈ�� �߰�������
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				
				}
				// �ڵ����� ����ġ�� ��
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f; //���콺 Ŭ���ð� �ʱ�ȭ
		bTargeting = false; // Ÿ���� ����
	}

}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//���ʸ��콺 ��ư �ƴϸ�
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		//�ɷ� ����
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	//Ÿ�ٿ� ���ٵǸ�
	if (bTargeting)
	{
		//�ɷ� ����
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			CachedDestination = Hit.ImpactPoint;
		}

		if (APawn* ControllerPawn = GetPawn())
		{
			//ControllerPawn�� ��ġ���� CachedDestination���� ���ϴ� ���� ���Ͱ� �˴ϴ�.
			const FVector WorldDirection = (CachedDestination - ControllerPawn->GetActorLocation()).GetSafeNormal();
			ControllerPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
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

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
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


