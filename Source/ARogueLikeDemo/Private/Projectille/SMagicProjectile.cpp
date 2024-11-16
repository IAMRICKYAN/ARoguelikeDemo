// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectille/SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"
#include "Actions/SActionComponent.h"
#include "Actions/SActionEffect.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMagicProjectile)


ASMagicProjectile::ASMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);

	InitialLifeSpan = 10.0f;
}

void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}



void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{

		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(GetParryTag()))
		{
			MovementComp->Velocity = -MovementComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		
		// Apply Damage & Impulse
		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, GetDamageAmount(), SweepResult))
		{
			Explode();

			if (ActionComp && HasAuthority() && GetBurningActionClass() )
			{
				ActionComp->AddAction(GetInstigator(), GetBurningActionClass());
			}
		}
	}
}

#if WITH_EDITOR
// Only required to convert existing properties already stored in Blueprints into the 'new' system
void ASMagicProjectile::MoveDataToSparseClassDataStruct() const
{
	// make sure we don't overwrite the sparse data if it has been saved already
	const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass == nullptr || BPClass->bIsSparseClassDataSerializable == true)
	{
		return;
	}
	
	Super::MoveDataToSparseClassDataStruct();

#if WITH_EDITORONLY_DATA
	// Unreal Header Tool (UHT) will create GetMySparseClassData automatically.
	FMagicProjectileSparseData* SparseClassData = GetMagicProjectileSparseData();

	// Modify these lines to include all Sparse Class Data properties.
	SparseClassData->DamageAmount = DamageAmount_DEPRECATED;
	SparseClassData->ParryTag = ParryTag_DEPRECATED;
	SparseClassData->BurningActionClass = BurningActionClass_DEPRECATED;
#endif // WITH_EDITORONLY_DATA
}
#endif