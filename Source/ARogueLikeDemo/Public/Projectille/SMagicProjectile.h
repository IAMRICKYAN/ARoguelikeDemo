// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Projectille/SProjectileBase.h"
#include "SMagicProjectile.generated.h"

/**
 * 
 */

class USActionEffect;

USTRUCT(BlueprintType)
struct FMagicProjectileSparseData : public FProjectileSparseData
{
	GENERATED_BODY()

	FMagicProjectileSparseData()
	: DamageAmount(20.f)
	{}
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> BurningActionClass;

};

UCLASS(SparseClassDataTypes = MagicProjectileSparseData)
class AROGUELIKEDEMO_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

	protected:
	
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostInitializeComponents() override;

public:
 	ASMagicProjectile();


#if WITH_EDITORONLY_DATA
	//~ These properties are moving out to the FMySparseClassData struct:
	
private:
	UPROPERTY()
	float DamageAmount_DEPRECATED;
	
	UPROPERTY()
	FGameplayTag ParryTag_DEPRECATED;
	
	UPROPERTY()
	TSubclassOf<USActionEffect> BurningActionClass_DEPRECATED;
	
#endif
	
#if WITH_EDITOR
public:
	// ~ This function transfers existing data into FMySparseClassData.
	virtual void MoveDataToSparseClassDataStruct() const override;
#endif
	
};
