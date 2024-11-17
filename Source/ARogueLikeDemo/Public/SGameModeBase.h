// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;

	//TSubclassOf<AActor> MonsterClass;

	/* 挑选这种怪物进行生成的相对权重 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* 游戏模式生成这种怪物需要的消耗 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* 给予击杀者的点数奖励  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
	

};

UCLASS()
class AROGUELIKEDEMO_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UDataTable> MonsterTable;

	FTimerHandle TimerHandle_SpawnBots;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	/* 花费点数生成Bot */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* SpawnCreditCurve;

	/* Time to wait between failed attempts to spawn/buy monster to give some time to build up credits. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float CooldownTimeBetweenFailures;

	// 蓝图拥有完整的读写权限，这样我们就可以通过难度曲线蓝图资产来修改这个值
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bAutoStartBotSpawning;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ruleset")
	bool bAutoRespawnPlayer;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	UEnvQuery* PowerupSpawnQuery;

	/* 使用EQS在关卡开始时生成的所有拾取物类 */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<AActor>> PowerupClasses;

	/* 拾取物之间的最小间隔距离 */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float RequiredPowerupDistance;

	/* 游戏开始时生成的拾取物的数量 */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 DesiredPowerupCount;

	// Points available to spend on spawning monsters
	float AvailableSpawnCredit;
	
	/* GameTime cooldown to give spawner some time to build up credits */
	float CooldownBotSpawnUntil;
	
	FMonsterInfoRow* SelectedMonsterRow;

	/* Allow per game mode actor pools and custom amount of primed instances */
	UPROPERTY(EditDefaultsOnly, Category= "Actor Pooling")
	TMap<TSubclassOf<AActor>, int32> ActorPoolClasses;
	

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable)
	void StartSpawningBots();
	
	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	void OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);
	
	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	
	void OnPowerupSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

public:
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void StartPlay() override;
	
	ASGameModeBase();
};
