// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SPowerup_Action.h"

#include "Actions/SAction.h"
#include "Actions/SActionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SPowerup_Action)


void ASPowerup_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);
	// 确保Instigator和要授予的Action都设置好了
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}

	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	// 检查玩家是否已经有这个类的技能了
	if (ActionComp)
	{
		if (ActionComp->GetAction(ActionToGrant))
		{
			//UE_LOG(LogTemp, Log, TEXT("Instigator already has action of class: %s"), *GetNameSafe(ActionToGrant));
			FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant));
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
			return;
		}

		// 授予新技能
		ActionComp->AddAction(InstigatorPawn, ActionToGrant);	
		HideAndCooldownPowerup();
	}
	
}