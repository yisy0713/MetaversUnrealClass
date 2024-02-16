// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	//Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangerControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove'"));
	if (nullptr != InputActionQuaterMoveRef.Object)
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Quater;
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	/*APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}*/

	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//향상된 입력 시스템 사용
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	//입력 맵핑컨텍스트에서의 액션이랑 함수랑 연결
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangerControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
}

void AABCharacterPlayer::AttackHitCheck(AttackType AttackType)
{
	//공격 충돌 판정을 한다.
	//UE_LOG(LogTemp, Log, TEXT("Attack Hit Check!"));

	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(Attack), false, this);
	FHitResult OutHitResult;
	const float AttackRange = 150.0f;
	const float CapsuleRadius = 50.0f;
	bool IsHit = false;

	TArray<FOverlapResult> LastComboAttackResult;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	const FVector LastComboAttackPos = GetActorLocation() + GetActorForwardVector() * (GetCapsuleComponent()->GetScaledCapsuleRadius() + AttackRange);
	const float LastComboAttackRange = 100.0f;
	const FVector LastComboAttacHalfSize(LastComboAttackRange, LastComboAttackRange, LastComboAttackRange/2);


	switch (AttackType)
	{
	case AttackType::Nomal:
		IsHit = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(CapsuleRadius), CollisionParams);
		break;
	case AttackType::Range:
		IsHit = GetWorld()->OverlapMultiByChannel(LastComboAttackResult, LastComboAttackPos, FQuat::Identity, ECC_GameTraceChannel1,
			FCollisionShape::MakeBox(LastComboAttacHalfSize), CollisionParams);
		//UE_LOG(LogTemp, Log, TEXT("%d"), IsHit);
		//UE_LOG(LogTemp, Log, TEXT("%d"), LastComboAttackResult.Num());
		/*IsHit = GetWorld()->OverlapMultiByChannel(LastComboAttack, LastComboAttackPos, FQuat::Identity, ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(LastComboAttackRange), CollisionParams);*/
		break;
	}
	//if (/*CurrentCombo == 4*/)            내꺼
	//{
	//	IsHit = GetWorld()->OverlapMultiByChannel(LastComboAttackResult, LastComboAttackPos, FQuat::Identity, ECC_GameTraceChannel1,
	//		FCollisionShape::MakeBox(LastComboAttacHalfSize), CollisionParams);
	//	//UE_LOG(LogTemp, Log, TEXT("%d"), IsHit);
	//	//UE_LOG(LogTemp, Log, TEXT("%d"), LastComboAttackResult.Num());
	//	/*IsHit = GetWorld()->OverlapMultiByChannel(LastComboAttack, LastComboAttackPos, FQuat::Identity, ECC_GameTraceChannel1,
	//		FCollisionShape::MakeSphere(LastComboAttackRange), CollisionParams);*/

	//}
	//else
	//{
	//	IsHit = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1,
	//		FCollisionShape::MakeSphere(CapsuleRadius), CollisionParams);
	//	//UE_LOG(LogTemp, Log, TEXT("%d"), IsHit);
	//}

	if (IsHit)
	{
		if (AttackType == AttackType::Range/*CurrentCombo == 4*/)
		{
			for (FOverlapResult HitResult : LastComboAttackResult)
			{
				FDamageEvent DamageEvent;
				HitResult.GetActor()->TakeDamage(80.f, DamageEvent, GetController(), this);
				//UE_LOG(LogTemp, Log, TEXT("%d"), LastComboAttackResult.Num());
			}
		}
		else
		{
			FDamageEvent DamageEvent;
			OutHitResult.GetActor()->TakeDamage(80.f, DamageEvent, GetController(), this);
		}
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsulePosition = Start + (End - Start) / 2.0f;
	float HalfHight = AttackRange / 2.0f;

	/*UE_LOG(LogTemp, Log, TEXT("%f %f %f"), CapsulePosition.X, CapsulePosition.Y, CapsulePosition.Z);
	UE_LOG(LogTemp, Log, TEXT("%f"), HalfHight);*/

	FColor Color = IsHit ? FColor::Green : FColor::Red;

	switch (AttackType)
	{
	case AttackType::Nomal:
		DrawDebugCapsule(GetWorld(), CapsulePosition, HalfHight, CapsuleRadius,
			FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), Color, false, 3.0f);
		break;
	case AttackType::Range:
		DrawDebugBox(GetWorld(), LastComboAttackPos, LastComboAttacHalfSize, Color, false, 3.0f);
		//DrawDebugSphere(GetWorld(), LastComboAttackPos, LastComboAttackRange, 8, FColor::Red, false, 3.0f);
		UE_LOG(LogTemp, Log, TEXT("%d"), LastComboAttackResult.Num());
		break;
	}

	//if (/*CurrentCombo == 4*/)				내꺼
	//{
	//	DrawDebugBox(GetWorld(), LastComboAttackPos, LastComboAttacHalfSize, FColor::Red, false, 3.0f);
	//	//DrawDebugSphere(GetWorld(), LastComboAttackPos, LastComboAttackRange, 14, FColor::Red, false, 3.0f);
	//	UE_LOG(LogTemp, Log, TEXT("%d"), LastComboAttackResult.Num());
	//}
	//else
	//{
	//	DrawDebugCapsule(GetWorld(), CapsulePosition, HalfHight, CapsuleRadius,
	//		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Red, false, 3.0f);
	//}
#endif

}

void AABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}

}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext,0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUesPawnControlRoation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollitionTest;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Square(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();

}
