// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMonsterData.generated.h"

class USAction;
/**
 * 
 */
UCLASS()
class AROGUELIKEDEMO_API USMonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TSubclassOf<AActor> MonsterClass;

	/* 授予怪物的技能和BUFF */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TArray<TSubclassOf<USAction>> Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Monsters", GetFName());
	}
	
};
