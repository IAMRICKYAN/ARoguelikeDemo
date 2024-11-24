// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SAnimInstance.h"

#include "SharedGameplayTags.h"
#include "Actions/SActionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAnimInstance)


void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	check(OwningActor);

	ActionComp = OwningActor->FindComponentByClass<USActionComponent>();
}



void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(SharedGameplayTags::Action_Stunned);
	}
}