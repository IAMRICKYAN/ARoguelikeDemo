// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SCharacter.generated.h"


class USInteractionComponent;
class USActionComponent;
class USAttributeComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class AROGUELIKEDEMO_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	
	FTimerHandle PrimaryAttackTimerHandle;

	float Intime;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;
	
UPROPERTY(VisibleAnywhere,Category = "Components" )
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere,Category = "Components")
	TObjectPtr<UCameraComponent> CameraComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USActionComponent> ActionComp;

	UPROPERTY(VisibleAnywhere,Category = "Components")
	TObjectPtr<USInteractionComponent> InteractionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	TObjectPtr<USAttributeComponent> AttributeComp;

	//Input System
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Basic")
	TObjectPtr<UInputAction> Input_Move;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Basic")
	TObjectPtr<UInputAction> Input_LookMouse;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Basic")
	TObjectPtr<UInputAction> Input_Jump;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Basic")
	TObjectPtr<UInputAction> Input_Sprint;

	//Attack
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Attack")
	TObjectPtr<UInputAction> Input_Attack;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Attack")
	TObjectPtr<UInputAction> Input_BlackHoleAttack;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Attack")
	TObjectPtr<UInputAction> Input_Dash;

	

	//Interaction
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput|Interaction")
	TObjectPtr<UInputAction> Input_Interact;

	virtual void PostInitializeComponents() override;

	void Action_Move(const FInputActionValue& InputValue);

	void Action_LookMouse(const FInputActionValue& InputValue);

	void Action_PrimaryAttack();

	void Action_PrimaryInteract();

	void Action_BlackHoleAttack();

	void Action_Dash();

	void Action_SprintStart();

	void Action_SprintStop();
public:

	UFUNCTION(Exec)
	void HealSelf(float amount = 100);
	
	UCameraComponent* GetCameraComponent();
	
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
