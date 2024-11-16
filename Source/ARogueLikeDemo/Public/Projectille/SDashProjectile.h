// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectille/SProjectileBase.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class AROGUELIKEDEMO_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	// 如果我们击中了物体,我们需要通过Handle来取消定时爆炸的计时器
	FTimerHandle TimerHandle_DelayedDetonate;

	// Base class using BlueprintNativeEvent, we must override the _Implementation not the Explode()
	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	virtual void BeginPlay() override;

public:

	ASDashProjectile();
};
