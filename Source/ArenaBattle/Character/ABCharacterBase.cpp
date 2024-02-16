// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h"
#include "UI/ABWidgetComponent.h"
#include "UI/ABUserWidget.h"
#include "Item/ABItemData.h"
#include "Item/ABWeaponItemData.h"
#include "Item/ABPotionItemData.h"
#include "Item/ABScrollItemData.h"


// Sets default values
AABCharacterBase::AABCharacterBase()
{
    //Stat Component
    Stat = CreateDefaultSubobject<UABCharactorStatComponent>(TEXT("Stat"));
    //UI Widget
    HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("HpBar"));
    HpBar->SetupAttachment(GetMesh());
    HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
    // Weapon Mesh Component
    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
    Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

    static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HPBar.WBP_HPBar_C"));
    if (HpBarWidgetRef.Class)
    {
        HpBar->SetWidgetClass(HpBarWidgetRef.Class);
        HpBar->SetWidgetSpace(EWidgetSpace::Screen);
        HpBar->SetDrawSize(FVector2D(150.0f, 20.0f));
        HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    //Pawn
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    //Capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCapsule"));

    //Movement
    GetCharacterMovement()->bOrientRotationToMovement = true;   //
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    //Mesh
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.f, 0.0f));
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharaterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
    if (CharaterMeshRef.Object)
    {
        GetMesh()->SetSkeletalMesh(CharaterMeshRef.Object);
    }

    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
    if (AnimInstanceClassRef.Class)
    {
        GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
    }

    static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/ChracterControl/ABC_Shoulder.ABC_Shoulder'"));
    if (ShoulderDataRef.Object)
    {
        CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
    }

    static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/ChracterControl/ABC_Quater.ABC_Quater'"));
    if (QuaterDataRef.Object)
    {
        CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);
    }

    Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);

    // Take Item Section
    TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
    TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
    TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
    if (InUserWidget)
    {
        InUserWidget->SetMaxHp(Stat->GetMaxHP());
        InUserWidget->UpdateHp(Stat->GetCurrentHP());
        Stat->OnHpChanged.AddUObject(InUserWidget, &UABUserWidget::UpdateHp);
    }
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharaterControlData)
{

    bUseControllerRotationYaw = CharaterControlData->bUseControllerRotationYaw;

    GetCharacterMovement()->bOrientRotationToMovement = CharaterControlData->bOrientRotationToMovement;
    GetCharacterMovement()->bUseControllerDesiredRotation = CharaterControlData->bUseControllerDesiredRotation;
    GetCharacterMovement()->RotationRate = CharaterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
    if (CurrentCombo == 0)
    {
        ComboActionBegin();
        return;
    }

    if (!CanComboInput)
    {
        ComboMiss = true;
        return;
    }
    if (ComboMiss)
    {
        return;
    }

    if (ComboTimerHandle.IsValid())                //타이머 세팅이 되지 않거나 타이머 세팅 시간이 지났을 때 
    {
        HasNextComboCommand = true;
    }
    else
    {
        HasNextComboCommand = false;
    }

    //if (ComboTimerHandle.IsValid() && !ComboStartTimerHandle.IsValid())               강사님꺼
    //{
    //    HasNextComboCommand = true;
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Log, TEXT("HasNextComboCommand = false"))
    //    HasNextComboCommand = false;
    //    GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
    //    ComboTimerHandle.Invalidate();
    //    GetWorld()->GetTimerManager().ClearTimer(ComboStartTimerHandle);
    //    ComboStartTimerHandle.Invalidate();
    //}

}

void AABCharacterBase::ComboActionBegin()
{
    //Combo Start
    CurrentCombo = 1;

    //movement setting
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    //animation
    const float AttackSpeedRate = 1.0f;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

    FOnMontageEnded EndDelegate;                                            //몽타주 끝날 때 호출가능한 델리게이트 선언
    EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);       //델리게이트에 엔드 이벤트 바인딩
    AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);  //바인딩 된 델리게이트 애니메이션 인스턴스에 세팅 

    ComboStartTimerHandle.Invalidate();
    ComboTimerHandle.Invalidate();
    SetDelayInputTimer();
    SetComboCheckTimer();

}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
    ensure(CurrentCombo != 0);
    CurrentCombo = 0;
    CanComboInput = false;
    ComboMiss = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
    int32 ComboIndex = CurrentCombo - 1;
    ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

    float ComboEffactiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate); //타이머 시간 초단위 (발동 원하는 프레임)/프레임단위(현재30)*재생속도
    if (ComboEffactiveTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffactiveTime, false);
    }
}

void AABCharacterBase::ComboCheck()
{
    ComboTimerHandle.Invalidate();

    if (HasNextComboCommand)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

        CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
        FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
        AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);

        SetComboCheckTimer();
        //SetDelayInputTimer();                         강사님꺼
        HasNextComboCommand = false;
    }
}

void AABCharacterBase::SetDelayInputTimer()
{
    float DelayTime = ComboActionData->EffectiveStartCount / ComboActionData->FrameRate;
    GetWorld()->GetTimerManager().SetTimer(ComboStartTimerHandle, this, &AABCharacterBase::ComboInputCheck, DelayTime, false);
}

void AABCharacterBase::SetDead()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    SetActorEnableCollision(false);

    //Dead Animation
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->StopAllMontages(0.0f);
    AnimInstance->Montage_Play(DeadMontage);

    GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([&]()
        {
            Destroy();
        }),3.0f, false);
}

//void AABCharacterBase::SetDelayInputTimer()           강사님꺼
//{
//    float DelayTime = ComboActionData->EffectiveStartCount / ComboActionData->FrameRate;
//    GetWorld()->GetTimerManager().SetTimer(ComboStartTimerHandle, this, &AABCharacterBase::SetComboCheckTimer, DelayTime, false);
//}

void AABCharacterBase::ComboInputCheck()
{
    CanComboInput = true;
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
    // 획득한 아이템 정보에 따라 추후 구현
    UE_LOG(LogTemp, Log, TEXT("%d"), InItemData->Type);

    //switch (InItemData->Type)
    //{
    //case EItemType::Potion:
    //	break;
    //case EItemType::Scroll:
    //	break;
    //case EItemType::Weapon:
    //	break;
    //}

    if (InItemData)
    {
        TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
    }
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
    UE_LOG(LogTemp, Log, TEXT("EquipWeapon"));

    UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData);
    if (WeaponItemData)
    {
        if (WeaponItemData->WeaponMesh.IsPending())
        {
            WeaponItemData->WeaponMesh.LoadSynchronous();
        }
        Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());
    }
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
    UE_LOG(LogTemp, Log, TEXT("DrinkPotion"));
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
    UE_LOG(LogTemp, Log, TEXT("ReadScroll"));
}

