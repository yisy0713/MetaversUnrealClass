// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

DECLARE_DELEGATE(FStageChangeDelegate);

USTRUCT(BlueprintType)
struct FStageChangeDelegateWrapper
{
	GENERATED_BODY()

	FStageChangeDelegateWrapper() {}
	FStageChangeDelegateWrapper(const FStageChangeDelegate& InDelegate) : StageDelegate (InDelegate) {}

	FStageChangeDelegate StageDelegate;
};

UENUM(BlueprintType)
enum class EStageState : uint8
{
	READY = 0,
	FIGHT,
	REWARD,
	NEXT
};

UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetState(EStageState InNewState);
	void SetReady();
	void SetFight();
	void SetReward();
	void SetNext();

	void OpenAllGate();
	void CloseAllGate();

protected:
	UPROPERTY(VisibleAnywhere, Category = Stage)
	TObjectPtr<class UStaticMeshComponent> Stage;

	UPROPERTY(VisibleAnywhere, Category = Stage)
	TObjectPtr<class UBoxComponent> StageTrigger;

	UPROPERTY(VisibleAnywhere, Category = Gate)
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;

	UPROPERTY(VisibleAnywhere, Category = Gate)
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	UPROPERTY(EditAnywhere, Category = Stage)
	EStageState CurrentState;

	UPROPERTY()
	TMap<EStageState, FStageChangeDelegateWrapper> StateChangeActions;

	TSubclassOf<class AABCharacterNonPlayer> OpponentClass;

	FTimerHandle OpponentSpawnTimer;

	UPROPERTY(EditAnywhere, Category = Fight)
	float OpponentSpawnTime;

	void OnOpponentSpawn();

	UFUNCTION()
	void OnOpponentDestroyed(AActor* DestroyedActor);

};
