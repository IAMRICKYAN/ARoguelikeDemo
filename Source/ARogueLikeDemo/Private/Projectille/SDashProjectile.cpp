// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectille/SDashProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SDashProjectile)



ASDashProjectile::ASDashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MovementComp->InitialSpeed = 6000.f;
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);

}

void ASDashProjectile::Explode_Implementation()
{
	//Super::Explode_Implementation();
	//如果是OnActorHit产生的爆炸，则不执行
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);
	
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Explode_Implementation"));
	
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);

	// 跳过基础实现,因为他会销毁Actor,而我们需要让投射物保留一段时间来触发第二个计时器
	//Super::Explode_Implementation();
	
}

void ASDashProjectile::TeleportInstigator()
{
	UE_LOG(LogTemp, Warning, TEXT("TeleportInstigator"));
	TObjectPtr<AActor> ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport))
	{
		UE_LOG(LogTemp, Warning, TEXT("TeleportInstigator1111"));
		// 保持玩家的旋转角度否则可能卡住
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);

		// Play shake on the player we teleported
		APawn* InstigatorPawn = Cast<APawn>(ActorToTeleport);
		APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
		if (PC && PC->IsLocalController())
		{
			PC->ClientStartCameraShake(ImpactShake);
		}
		
	}
	Destroy();
}