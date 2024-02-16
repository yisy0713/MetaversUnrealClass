// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetOwinngActor(AActor* NewOwner) { OwningActor = NewOwner;}
	FORCEINLINE void SetMaxHp(float NewHp) { MaxHp = NewHp; }
	void UpdateHp(float NewHp);
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor)
	TObjectPtr<AActor> OwningActor;

	TObjectPtr<class UProgressBar> HpProgressBar;
	float MaxHp;
};
