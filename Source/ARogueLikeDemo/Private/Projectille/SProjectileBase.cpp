
// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectille/SProjectileBase.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SProjectileBase)

//注意：使用 SparseDataClass 功能时，某些属性将替换为由 UHT 自动生成的“GetXXX（）”。
//示例：DamageAmount 变为 GetDamageAmount（），而此函数在我们自己的标头中不可见。

ASProjectileBase::ASProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Projectile");
	// Dont bother telling the nav system whenever we move
	SphereComp->SetCanEverAffectNavigation(false);
	SphereComp->InitSphereRadius(12.0f);
	RootComponent = SphereComp;

	ImpactVFX = CreateDefaultSubobject<UParticleSystem>(TEXT("ImpactVFX"));
	

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(SphereComp);

	

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);

	// Custom Projectile Component (for tick management & better homing)
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMoveComp");
	MovementComp->InitialSpeed = 4000.0f;
	MovementComp->MaxSpeed = 8000.0f;
	//这表明物体的旋转将会跟随其速度向量。也就是说，物体将会朝它移动的方向旋转，
	//这对于模拟例如子弹或者火箭这样的物体是很常见的，这些物体的尾部通常指向它们移动的反方向。
	MovementComp->bRotationFollowsVelocity = true;
	//这表示物体的初始速度是相对于物体自身的局部空间来定义的，而不是世界空间。
	//局部空间意味着速度向量是相对于物体自身的方向和坐标系统。如果这个属性设置为false，那么初始速度将会在世界空间中定义，与物体的朝向无关。
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->Bounciness = 0.3f;
	MovementComp->ProjectileGravityScale = 0.0f;
	
	//SetReplicates(true);
	bReplicates = true;

}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
	//SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if((OtherActor!=nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if( OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity()*120.0f, GetActorLocation());
		}
		Explode();
	}

}

void ASProjectileBase::Explode_Implementation()
{
	if(ensure(IsValid(this)))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation(), true, EPSCPoolMethod::AutoRelease);
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
		UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(),GetImpactShakeInnerRadius(),GetImpactShakeOuterRadius());
		Destroy();
	}
}


// Called when the game starts or when spawned
void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Prepare for pool by disabling and resetting for the next time it will be re-used
	{
		// Complete any active PSCs (These may be pooled themselves by the particle manager) - old Cascade system since we dont use Niagara yet
		TInlineComponentArray<UParticleSystemComponent*> ParticleComponents(this);
		for (UParticleSystemComponent* const PSC  : ParticleComponents)
		{
			PSC->Complete();
		}
	}
}


#if WITH_EDITOR
// 只需将蓝图中已存储的现有属性转换为“新”系统
void ASProjectileBase::MoveDataToSparseClassDataStruct() const
{
	// 确保我们不会覆盖已保存的稀疏数据
	const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass == nullptr || BPClass->bIsSparseClassDataSerializable == true)
	{
		return;
	}
	
	Super::MoveDataToSparseClassDataStruct();
#if WITH_EDITORONLY_DATA
	// Unreal Header Tool （UHT） 将自动创建 GetMySparseClassData。
	FProjectileSparseData* SparseClassData = GetProjectileSparseData();
	// 修改这些行以包括所有 Sparse Class Data 属性。
	SparseClassData->ImpactShakeInnerRadius = ImpactShakeInnerRadius_DEPRECATED;
	SparseClassData->ImpactShakeOuterRadius = ImpactShakeOuterRadius_DEPRECATED;
#endif // WITH_EDITORONLY_DATA
}
#endif