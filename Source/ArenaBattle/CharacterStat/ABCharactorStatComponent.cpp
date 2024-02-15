// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharactorStatComponent.h"

// Sets default values for this component's properties
UABCharactorStatComponent::UABCharactorStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UABCharactorStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	SetMaxHP(100.0f);
	SetHP(GetMaxHP());
	
}

void UABCharactorStatComponent::SetMaxHP(float NewHP)
{
	MaxHp = NewHP;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHp);
}

void UABCharactorStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP,0.0f,MaxHp);

	OnHpChanged.Broadcast(CurrentHP);
}


// Called every frame
void UABCharactorStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UABCharactorStatComponent::ApplyDamage(float InDamage)
{
	float ActualDamage = FMath::Clamp(InDamage, 0.0f, InDamage);

	SetHP(CurrentHP - ActualDamage);
	if (CurrentHP <= 0.0f)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

