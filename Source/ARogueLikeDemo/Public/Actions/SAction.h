// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"

class USActionComponent;
/**
 * 
 */
USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool bIsRunning;
	
	UPROPERTY()
	AActor* Instigator;
	
	FActionRepData()
	{
		bIsRunning = false;
	}
};

UCLASS(Blueprintable)
class AROGUELIKEDEMO_API USAction : public UObject
{
	GENERATED_BODY()

	protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Replicated)
	TObjectPtr<USActionComponent> ActionComp;
	
	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;
	
	UFUNCTION()
	void OnRep_RepData();
	
	/* Tags added to owning actor when activated, removed when action stops */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	/* Action can only start if OwningActor has none of these Tags applied */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	UPROPERTY(Replicated)
	float TimeStarted;
	
public:
	/* 当我们将这个技能添加到ActionComponent中时自动执行 */
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;
	
	UFUNCTION(BlueprintNativeEvent,Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable,Category="Action")
	bool IsRunning() const;
	

	void Initialize(USActionComponent* NewActionComp);
	

	UFUNCTION(BlueprintNativeEvent,Category="Action")
	bool CanStart(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly,Category="Action")
	FGameplayTag ActivationTag;
	
	UWorld* GetWorld() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
};
