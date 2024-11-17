// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAIController)


void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);

	
}