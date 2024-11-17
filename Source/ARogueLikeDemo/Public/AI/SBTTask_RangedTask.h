// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_RangedTask.generated.h"

/**
 * 
 */
UCLASS()
class AROGUELIKEDEMO_API USBTTask_RangedTask : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category= "AI")
	FName MuzzleSocket;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxBulletSpread;

public:
	USBTTask_RangedTask();	
};
