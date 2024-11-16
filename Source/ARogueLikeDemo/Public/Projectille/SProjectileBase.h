// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

USTRUCT(BlueprintType)
struct FProjectileSparseData
{
	GENERATED_BODY()
	FProjectileSparseData()
	: ImpactShakeInnerRadius(0.f),
	ImpactShakeOuterRadius(1500.f)
	{ }
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;
};

UCLASS(ABSTRACT, SparseClassDataTypes = ProjectileSparseData)
class AROGUELIKEDEMO_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
protected:
	

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UProjectileMovementComponent> MovementComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(VisibleAnywhere,Category="Effects")
	TObjectPtr<UParticleSystemComponent> EffectComp;
	
	UPROPERTY(EditDefaultsOnly,Category="Effects")
	TObjectPtr<UParticleSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category="Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	


	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void Explode();
	


	virtual void PostInitializeComponents() override;

public:
	// Sets default values for this actor's properties
	ASProjectileBase();

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
#if WITH_EDITORONLY_DATA
	//~ These properties are moving out to the FMySparseClassData struct:
private:
	
	UPROPERTY()
	float ImpactShakeInnerRadius_DEPRECATED;
	
	UPROPERTY()
	float ImpactShakeOuterRadius_DEPRECATED;
	
#endif
	
#if WITH_EDITOR
public:
	// ~ This function transfers existing data into FMySparseClassData.
	virtual void MoveDataToSparseClassDataStruct() const override;
#endif

};
