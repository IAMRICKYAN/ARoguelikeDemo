// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "ARogueLikeDemo.h"
#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "Actions/SAction.h"
#include "Actions/SActionComponent.h"
#include "AI/SAICharacter.h"
#include "Data/SMonsterData.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/GameplayStatics.h"
#include "Save/SSaveGameSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SGameModeBase)

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);




ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	CreditsPerKill = 20;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	PlayerStateClass = ASPlayerState::StaticClass();

	bAutoStartBotSpawning = false;

	bAutoRespawnPlayer = false;

	CooldownTimeBetweenFailures = 8.0f;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	// （保存/加载逻辑已移至新的 SaveGameSubsystem 中）
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();

	// 可选插槽名称（否则回退到 SaveGameSettings class/INI 中指定的插槽）
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);
	
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	SG->OverrideSpawnTransform(NewPlayer);
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (bAutoStartBotSpawning)
	{
		StartSpawningBots();
	}
	
	// 只有在数组中添加了开局时生成的拾取物类时才会执行这一段逻辑
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Skip the Blueprint wrapper and use the direct C++ option which the Wrapper uses as well
		FEnvQueryRequest Request(PowerupSpawnQuery, this);
		Request.Execute(EEnvQueryRunMode::AllMatching, this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
	}
	
}


void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		// Auto-respawn
		if (bAutoRespawnPlayer)
		{
			FTimerHandle TimerHandle_RespawnDelay;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &ThisClass::RespawnPlayerElapsed, Player->GetController());
 
			const float RespawnDelay = 2.0f;
			GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
		}

		// Store time if it was better than previous record
		ASPlayerState* PS = Player->GetPlayerState<ASPlayerState>();
		if (PS)
		{
			PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		}

		USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
		// Immediately auto save on death
		SG->WriteSaveGame();
	}

	// Give Credits for kill
	APawn* KillerPawn = Cast<APawn>(Killer);
	// Don't credit kills of self
	if (KillerPawn && KillerPawn != VictimActor)
	{
		// Only Players will have a 'PlayerState' instance, bots have nullptr
		ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();
		if (PS) 
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}



void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}


void ASGameModeBase::KillAll()
{
	for (ASAICharacter* Bot : TActorRange<ASAICharacter>(GetWorld()))
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void ASGameModeBase::StartSpawningBots()
{
	if (TimerHandle_SpawnBots.IsValid())
	{
		// Already spawning bots.
		return;
	}
	
	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether its allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{

	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}

	// Give points to spend
	if (SpawnCreditCurve)
	{
		AvailableSpawnCredit += SpawnCreditCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (CooldownBotSpawnUntil > GetWorld()->TimeSeconds)
	{
		// Still cooling down
		return;
	}

	LogOnScreen(this, FString::Printf(TEXT("Available SpawnCredits: %f"), AvailableSpawnCredit));
	

	int32 NrOfAliveBots = 0;
	
	// TActorRange simplifies the code compared to TActorIterator<T
	for (ASAICharacter* Bot : TActorRange<ASAICharacter>(GetWorld()))
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
		
	}

	const float MaxBotCount = 8.0f;

	UE_LOG(LogTemp, Log, TEXT("Alive Bots %i"), NrOfAliveBots);
	
	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}

	if (MonsterTable)
	{
		// Reset before selecting new row
		SelectedMonsterRow = nullptr;

		TArray<FMonsterInfoRow*> Rows;
		MonsterTable->GetAllRows("", Rows);

		// Get total weight
		float TotalWeight = 0;
		for (FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;
		}

		// Random number within total random
		int32 RandomWeight = FMath::RandRange(0.0f, TotalWeight);

		//Reset
		TotalWeight = 0;

		// Get monster based on random weight
		for (FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;

			if (RandomWeight <= TotalWeight)
			{
				SelectedMonsterRow = Entry;
				break;
			}
		}

		if (SelectedMonsterRow && SelectedMonsterRow->SpawnCost >= AvailableSpawnCredit)
		{
			// Too expensive to spawn, try again soon
			CooldownBotSpawnUntil = GetWorld()->TimeSeconds + CooldownTimeBetweenFailures;

			LogOnScreen(this, FString::Printf(TEXT("Cooling down until: %f"), CooldownBotSpawnUntil), FColor::Red);
			return;
		}
	}

	// Run EQS to find valid spawn location
	// Skip the Blueprint wrapper and use the direct C++ option which the Wrapper uses as well
	FEnvQueryRequest Request(SpawnBotQuery, this);
	Request.Execute(EEnvQueryRunMode::RandomBest5Pct, this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	FEnvQueryResult* QueryResult = Result.Get();
	if (!QueryResult->IsSuccessful())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	// Retrieve all possible locations that passed the query
	TArray<FVector> Locations;
	QueryResult->GetAllAsLocations(Locations);
	
	// 把之前使用过的位置储存起来，这样我们可以更快地计算出距离
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	// 如果我们已经生成了足够多的拾取物或者没有更多的可以使用的生成点时结束
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// 从剩下的殿中随机选出一个
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];
		// 将选出的点删除避免重复使用
		Locations.RemoveAt(RandomLocationIndex);

		// 检查最小距离
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				// 可以选择使用Debug信息显示出因为距离而被筛选掉的点
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// 点之间距离太近，挑选其他的
				bValidLocation = false;
				break;
			}
		}

		// 没有通过距离测试
		if (!bValidLocation)
		{
			continue;
		}

		// 随机挑选一个拾取物类
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// 将使用过的点储存起来用于距离测试
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	FEnvQueryResult* QueryResult = Result.Get();
	if (!QueryResult->IsSuccessful())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}
	
	// 检索通过查询的所有可能位置
	TArray<FVector> Locations;
	QueryResult->GetAllAsLocations(Locations);
	if (Locations.IsValidIndex(0) && MonsterTable)
	{
		UAssetManager& Manager = UAssetManager::Get();
		
		// Apply spawn cost
		AvailableSpawnCredit -= SelectedMonsterRow->SpawnCost;

		FPrimaryAssetId MonsterId = SelectedMonsterRow->MonsterId;

		TArray<FName> Bundles;
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, MonsterId, Locations[0]);
		Manager.LoadPrimaryAsset(MonsterId, Bundles, Delegate);

	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	//LogOnScreen(this, "Finished loading.", FColor::Green);

	UAssetManager& Manager = UAssetManager::Get();

	USMonsterData* MonsterData = CastChecked<USMonsterData>(Manager.GetPrimaryAssetObject(LoadedId));
	
	AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
	// Spawn might fail if colliding with environment
	if (NewBot)
	{
		LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

		// Grant special actions, buffs etc.
		USActionComponent* ActionComp = NewBot->FindComponentByClass<USActionComponent>();
		check(ActionComp);
		
		for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
		{
			ActionComp->AddAction(NewBot, ActionClass);
		}
	}
}