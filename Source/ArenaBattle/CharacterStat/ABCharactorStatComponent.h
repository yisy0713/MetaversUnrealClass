// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABCharactorStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharactorStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharactorStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetMaxHP(float NewHP);
	void SetHP(float NewHP);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	float ApplyDamage(float InDamage);

	FORCEINLINE float GetMaxHP() { return MaxHp; }
	FORCEINLINE float GetCurrentHP() { return CurrentHP; }

public:

	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

protected:

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)/*Transient 직렬화에서 제외하겠다는*/
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)/*Transient 직렬화에서 제외하겠다는*/
	float CurrentHP;
		
};
