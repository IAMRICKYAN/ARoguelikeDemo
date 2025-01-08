// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SItemChest.h"

#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SItemChest)

// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	Treasure = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Treasure"));
	Treasure->SetupAttachment(BaseMesh);
	

	VFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VFX"));
	VFX->SetupAttachment(Treasure);
	VFX->bAutoActivate = false;

	bIsLidOpened = false;
	TargetPitch = 120.f;

	//SetReplicates(true);
	bReplicates = true;
	
	
}

//这里的ReplicatedUsing可以这样简单理解：Server的Fired值变化时，Client中会触发函数OnRep_OnFire()，
//但是在Server需要手动调用OnRep_OnFire()。

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	ISGameplayInterface::Interact_Implementation(InstigatorPawn);
	bIsLidOpened = !bIsLidOpened;
	OnRep_LidOpened();  //这就是为什么我们需要手动写这个的原因
}

void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
	
}

void ASItemChest::OnRep_LidOpened()
{
	float CurrentPitch = bIsLidOpened ? TargetPitch : 0.f;
	LidMesh->SetRelativeRotation(FRotator(CurrentPitch, 0, 0));
}


void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASItemChest, bIsLidOpened);
}

