// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_StartAction.h"

#include "AIController.h"
#include "Actions/SActionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SBTTask_StartAction)


EBTNodeResult::Type USBTTask_StartAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		APawn* MyPawn = MyController->GetPawn();
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		USActionComponent* ActionComp = Cast<USActionComponent>(MyPawn->GetComponentByClass(USActionComponent::StaticClass()));
		if (ensure(ActionComp))
		{
			if (ActionComp->StartActionByName(MyPawn, ActionName))
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
	
}
