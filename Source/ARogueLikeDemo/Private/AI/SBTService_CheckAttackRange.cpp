// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SBTService_CheckAttackRange)




USBTService_CheckAttackRange::USBTService_CheckAttackRange()
{
	TargetActorKey.SelectedKeyName = "TargetActor";
	MaxAttackRange = 500.0f;
}

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between ai pawn and target actor
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if(TargetActor)
		{
			AAIController* MyAIController = Cast<AAIController>(OwnerComp.GetAIOwner());
			if(ensure(MyAIController))
			{
				APawn* AIPawn = MyAIController->GetPawn();

				if(ensure(AIPawn))
				{
					float DistanceTo = FVector::Distance(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());

					bool bWithinRange = DistanceTo < MaxAttackRange;
					
					bool bHasLos = false;

					if(bWithinRange)
					{
						bHasLos = MyAIController->LineOfSightTo(TargetActor);
					}

					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bHasLos);
				}
			}
		}
	}
}