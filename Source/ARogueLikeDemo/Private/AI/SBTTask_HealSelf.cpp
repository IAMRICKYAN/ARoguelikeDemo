// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealSelf.h"

#include "AIController.h"
#include "SAttributeComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SBTTask_HealSelf)


EBTNodeResult::Type USBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(MyPawn);

	if(ensure(AttributeComp))
	{
		AttributeComp->ApplyHealthChanged(MyPawn, AttributeComp->GetHealthMax());
	}

	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}