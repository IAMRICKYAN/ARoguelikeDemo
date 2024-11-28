// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectille/SAction_ProjectileAttack.h"

#include "ARogueLikeDemo.h"
#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAction_ProjectileAttack)


USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;

	SweepRadius = 20.0f;
	SweepDistanceFallback = 5000;
}



void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	
	Super::StartAction_Implementation(Instigator);
	
	
	if(TObjectPtr<ACharacter> Character = Cast<ACharacter>(Instigator))
	{
		Character->PlayAnimMontage(AttackAnim);
		// Auto-released particle pooling
		UGameplayStatics::SpawnEmitterAttached(ImpactVFX, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, true, EPSCPoolMethod::AutoRelease);
		UGameplayStatics::SpawnSoundAttached(CastingSound, Character->GetMesh());
		
		if(Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

			//GetWorld() 被调用来获取当前 USAction_ProjectileAttack 实例所在的游戏世界（UWorld）的引用。
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
		
		

	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	

	if(ensureAlways(ProjectileClass))
	{
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(SweepRadius);

		// Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);


		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		//获得Cameracomponent
		TObjectPtr<ASCharacter> Player = Cast<ASCharacter>(InstigatorCharacter);
		UCameraComponent * CameraComp = Player->GetCameraComponent();
		const FVector TraceStart = CameraComp->GetComponentLocation();
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		const FVector TraceEnd = CameraComp->GetComponentLocation() + (Player->GetControlRotation().Vector() * 5000);
		FVector AdjustedTraceEnd = TraceEnd;
		

		TArray<FHitResult> Hits;
		// returns true if we got to a blocking hit
		if (GetWorld()->SweepMultiByChannel(Hits, TraceStart, TraceEnd, FQuat::Identity, COLLISION_PROJECTILE, Shape, Params))
		{
			//bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params);
			//FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
			//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, SweepRadius, 16, LineColor, false, 5);		
			// Overwrite trace end with impact point in world
			//TraceEnd = Hits.ImpactPoint;
			AdjustedTraceEnd = Hits[0].ImpactPoint;
		}
		FRotator ProjRotation = (AdjustedTraceEnd - HandLocation).Rotation();

		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		
		// 在世界中生成
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
	StopAction(InstigatorCharacter);
}