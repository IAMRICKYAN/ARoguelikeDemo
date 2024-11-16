// Fill out your copyright notice in the Description page of Project Settings.


#include "ARogueLikeDemo/Public/SCharacter.h"

#include "GameplayTagContainer.h"
#include "SAttributeComponent.h"
#include "SharedGameplayTags.h"
#include "SInteractionComponent.h"
#include "Actions/SActionComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(SCharacter)


// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	// 我们已经用 pawn 控制旋转来控制弹簧臂的旋转，这禁用了一个微妙的副作用
	// 其中，旋转我们的胶囊体组件（例如，由角色移动中的 bOrientRotationToMovement 引起）将旋转我们的弹簧臂，直到它在更新后期自我校正
	// 在 Tick 期间使用 CameraLocation 时，这可能会导致不需要的效果，因为它可能会略微偏离我们的最终摄像机位置。
	SpringArmComp->SetUsingAbsoluteRotation(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	CameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

	ActionComp=CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	TimeToHitParamName = "TimeToHit";
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributeComp->OnHealthChanged.AddDynamic(this,&ASCharacter::OnHealthChanged);
	Intime =0.2f;
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(APlayerController* PlayerController = CastChecked<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//将增强输入子系统与我们创建的映射文件绑定
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext,0);
			
		}
	}

	//映射文件添加好后，我们还要与我们的Action进行绑定
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(Input_LookMouse,ETriggerEvent::Triggered,this,&ASCharacter::Action_LookMouse);
		EnhancedInputComponent->BindAction(Input_Move,ETriggerEvent::Triggered,this,&ASCharacter::Action_Move);
		EnhancedInputComponent->BindAction(Input_Jump,ETriggerEvent::Triggered,this,&ACharacter::Jump);
		EnhancedInputComponent->BindAction(Input_Attack,ETriggerEvent::Triggered,this,&ASCharacter::Action_PrimaryAttack);
		EnhancedInputComponent->BindAction(Input_Interact,ETriggerEvent::Triggered,this,&ASCharacter::Action_PrimaryInteract);
		EnhancedInputComponent->BindAction(Input_BlackHoleAttack,ETriggerEvent::Triggered,this,&ASCharacter::Action_BlackHoleAttack);
		EnhancedInputComponent->BindAction(Input_Dash,ETriggerEvent::Triggered,this,&ASCharacter::Action_Dash);
		
		EnhancedInputComponent->BindAction(Input_Sprint, ETriggerEvent::Started, this, &ASCharacter::Action_SprintStart);
		EnhancedInputComponent->BindAction(Input_Sprint, ETriggerEvent::Completed, this, &ASCharacter::Action_SprintStop);

	}
}

void ASCharacter::Action_Move(const FInputActionValue& InputValue)
{
	FRotator ControlRot = GetControlRotation();
	//只与偏航角Yaw有关，其余设置为0
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	//从输入得到二维向量
	const FVector2D AxisValue = InputValue.Get<FVector2D>();

	//前后移动
	AddMovementInput(ControlRot.Vector(),AxisValue.Y);

	//左右移动
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector,AxisValue.X);
}

void ASCharacter::Action_LookMouse(const FInputActionValue& InputValue)
{
	const FVector2d Value = InputValue.Get<FVector2d>();
	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void ASCharacter::Action_PrimaryAttack()
{
	//ActionComp->StartActionByName(this,"PrimaryAttack");
	static FGameplayTag ActivationTag = FGameplayTag::RequestGameplayTag("Action.PrimaryAttack");

	ActionComp->StartActionByName(this,ActivationTag);
}

void ASCharacter::Action_PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->Interact();
	}
}

void ASCharacter::Action_BlackHoleAttack()
{
	static FGameplayTag ActivationTag = FGameplayTag::RequestGameplayTag("Action.Blackhole");
	ActionComp->StartActionByName(this,ActivationTag);
}

void ASCharacter::Action_Dash()
{
	static FGameplayTag ActivationTag = FGameplayTag::RequestGameplayTag("Action.Dash");
	ActionComp->StartActionByName(this,ActivationTag);
}

void ASCharacter::Action_SprintStart()
{
	ActionComp->StartActionByName(this, SharedGameplayTags::Action_Sprint);
}

void ASCharacter::Action_SprintStop()
{
	ActionComp->StopActionByName(this, SharedGameplayTags::Action_Sprint);
}

void ASCharacter::HealSelf(float amount)
{
	AttributeComp->ApplyHealthChanged(this,amount);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// Rage added equal to damage received (Abs to turn into positive rage number)
		float RageDelta = FMath::Abs(Delta);
		AttributeComp->ApplyRage(InstigatorActor, RageDelta);
	}
	
	if(Delta<0.0f && NewHealth <= 0.0f)
	{
			APlayerController* PC = Cast<APlayerController>(GetController());

			DisableInput(PC);

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetLifeSpan(5.0f);
		
	}
}

UCameraComponent* ASCharacter::GetCameraComponent()
{
	return CameraComp;
}