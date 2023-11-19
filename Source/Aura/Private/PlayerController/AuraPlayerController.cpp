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
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

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

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) &&  DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		// 컴포넌트를 엔진에 등록합니다.
		DamageText->RegisterComponent();

		// 대상 캐릭터의 루트 컴포넌트에 데미지 텍스트를 연결합니다.
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		// 데미지 텍스트를 대상 캐릭터에 붙인 후, 분리합니다. (애니메이션을 위해)
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		// 데미지 텍스트에 표시할 데미지 양을 설정합니다.
		DamageText->SetDamageText(DamageAmount,bBlockedHit,bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	//클라이언트도 실행하라면 에디터에서 프로젝트 세팅 Nvation System-> Allow Clinet Side Navigation 체크해줘야함
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		//ControlledPawn의 위치를 기반으로 Spline 상에서 가장 가까운 위치를 찾아 LocationOnSpline에 저장합니다. 
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// LocationOnSpline 위치에서 가장 가까운 지점의 방향을 찾아내줌
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		//그 방향으로 이동
		ControlledPawn->AddMovementInput(Direction);

		//캐릭터 위치에서 다음 Spline위치 사이 거리
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();

		//위치에 도착하면
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor =Cast<IEnemyInterface>(CursorHit.GetActor());

	/** 커서 라인 트레이스
	* A.마지막 엑터가 널 && 지금 액터도 널 (적을 안가리키고 있음)
	*   -아무것도 하지않음
	* B. LastActor 널 && 지금 액터 값이 있음 (적을 가리키는중)
	*	-하이라이트 thisActor
	* C. LastActor 값이 있고 && ThisActor 널 (적에서 마우스를 옮김)
	*  - LastActor 하이라이트 해제
	* D. 둘다 값이 있고, 서로 다른 값일때 (다른 적으로 옮김)
	*  - Last 하이타이트 해제 , thisActor 하이라트
	* E. 둘다 값이 있고, 서로 값은 값 (적을 가리키고 있다는 뜻)
	*  - Return
	*/

	if (LastActor != ThisActor)  //서로 다르면
	{
		if (LastActor) LastActor->UnHighlightActor(); 
		if (ThisActor) ThisActor->HighlightActor();
	}

}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		/** 타켓이 있으면 트루*/
		bTargeting = ThisActor ? true : false;
		/** 그쪽으로 가면 안 됨*/
		bAutoRunning = false;
	}

}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//왼쪽마우스 버튼 아니면
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		//능력 비활성화
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	//능력 비활성화
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);


	//타겟에 마우스포인트가 없고 쉬프트키를 안눌렀으면
	if (!bTargeting || !bShiftKeyDown)
	{
		const APawn* ControllerPawn = GetPawn();
		//짧게 눌렀으면 그 지역으로 가야됨
		if (FollowTime <= ShortPressThreshold && ControllerPawn)
		{
			/**
			* 내비게이션 시스템에서 특정 위치로 경로를 동기적으로 찾는 함수입니다.
			*이 함수는 주어진 시작 위치에서 목적지 위치까지의 경로를 계산하고 그 결과를 즉시 반환합니다.
			*동기적이라는 것은 함수가 경로를 계산할 때까지 대기한다는 것을 의미합니다.
			*따라서 이 함수가 완료될 때까지 프로그램이 다음 코드로 진행하지 않습니다.
			*/
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControllerPawn->GetActorLocation(), CachedDestination))
			{
				//포인트를 지워줌
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					//스필라인 포인트를 추가시켜줌
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);

				}
				if (NavPath->PathPoints.Num() > 0)
				{
					//캐릭터 클릭 위치를 마지막 지점으로 해줌
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					// 자동으로 그위치로 감
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f; //마우스 클릭시간 초기화
		bTargeting = false; // 타겟은 없음
	}

}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//왼쪽마우스 버튼 아니면
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		//능력 실행
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	//타겟에 갖다되거나 쉬프티 키를 누르면
	if (bTargeting || bShiftKeyDown)
	{
		//능력 실행
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControllerPawn = GetPawn())
		{
			//ControllerPawn의 위치에서 CachedDestination까지 향하는 방향 벡터가 됩니다.
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
	DefaultMouseCursor = EMouseCursor::Default; //마우스커서 모드

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
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftRelease);

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
		ControllerdPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); //에디터에서 Y
		ControllerdPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}


