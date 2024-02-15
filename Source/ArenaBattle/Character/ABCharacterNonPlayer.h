// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase, public IABAnimationAttackInterface
{
	GENERATED_BODY()
	
public:
	AABCharacterNonPlayer();
	
protected:
	virtual void AttackHitCheck(AttackType AttackType) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator,AActor* DamageCauser) override;
};
