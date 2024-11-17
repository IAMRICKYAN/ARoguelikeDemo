// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class USSaveGame*, SaveObject);

UCLASS(meta=(DisplayName="SaveGame System"))
class AROGUELIKEDEMO_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	
	/* 要保存/加载到磁盘的插槽的名称。由 SaveGameSettings 填充（可以从 GameMode 的 InitGame（） 中覆盖） */
	FString CurrentSlotName;
	
	UPROPERTY()
	USSaveGame* CurrentSaveGame;
	
public:
	/* 将 PlayerState 中的序列化数据恢复到 player 中 */
	void HandleStartingNewPlayer(AController* NewPlayer);
	
	/* 在完全初始化后，使用每个 PlayerState 的存储数据恢复生成变换。 */
	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(AController* NewPlayer);
	
	/* 更改插槽名称，下次加载/保存数据时将使用 */
	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);
	
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();
	
	/* 从磁盘加载，可选插槽名称 */
	void LoadSaveGame(FString InSlotName = "");
	
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;
	
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;
	
	/* 初始化子系统，在 SaveGameSettings 变量中加载的好时机 */
	void Initialize(FSubsystemCollectionBase& Collection) override;
};
