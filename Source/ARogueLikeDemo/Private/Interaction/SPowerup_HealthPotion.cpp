// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SPowerup_HealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SPowerup_HealthPotion)



#define LOCTEXT_NAMESPACE "InteractableActors"

class ASPlayerState;
class USAttributeComponent;

ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	CreditCost = 50;
}



void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	// 检查使用者是否生命值已满
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		if(ASPlayerState* PS = InstigatorPawn->GetPlayerState<ASPlayerState>())
		{
			// 只在成功治疗时进入冷却
			if (PS->RemoveCredits(CreditCost) && AttributeComp->ApplyHealthChanged(this, AttributeComp->GetHealthMax()))
			{
				HideAndCooldownPowerup();
			}
			
		}
	}
}

FText ASPowerup_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);

	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "你的血量是满的.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "花费 {0} 点数. 恢复至满血."), CreditCost);
}


#undef LOCTEXT_NAMESPACE