// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class AROGUELIKEDEMO_API ASItemChest : public AActor,public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASItemChest();

	UFUNCTION()
	void Interact_Implementation(APawn* InstigatorPawn);

	UFUNCTION()
	void OnActorLoaded_Implementation();
	
	UPROPERTY(EditDefaultsOnly)
	float TargetPitch;

protected:

	UFUNCTION()
	void OnRep_LidOpened();

	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing="OnRep_LidOpened", SaveGame)
	bool bIsLidOpened;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UStaticMeshComponent> LidMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UStaticMeshComponent> Treasure;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UParticleSystemComponent> VFX;
	

};
