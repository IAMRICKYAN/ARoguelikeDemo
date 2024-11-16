// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Online/CoreOnline.h"
#include "SSaveGame.generated.h"


USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:

	/* 标记这些信息属于哪个Actor */
	UPROPERTY()
	//FString ActorName;
	FName ActorName;

	/* 对于可以移动的Actor保存变换信息 */
	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()
public:

	FPlayerSaveData()
	{
		Credits = 0;
		PersonalRecordTime = 0.0f;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		bResumeAtTransform = true;
	}
	
	/* Player Id defined by the online sub system. */ 
	/*UPROPERTY()
	FString PlayerID;*/

	UPROPERTY()
	FUniqueNetIdRepl PlayerID;
	
	UPROPERTY()
	int32 Credits;
	
	/* Longest survival time */
	UPROPERTY()
	float PersonalRecordTime;
	
	/* Location if player was alive during save */
	UPROPERTY()
	FVector Location;
	
	/* Orientation if player was alive during save */ 
	UPROPERTY()
	FRotator Rotation;
	
	/* We don't always want to restore location, and may just resume player at specific respawn point in world. */
	UPROPERTY()
	bool bResumeAtTransform;
};

UCLASS()
class AROGUELIKEDEMO_API USSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TArray<FPlayerSaveData> SavedPlayers;

	/* Actors stored from a level (currently does not support a specific level and just assumes the demo map) */
	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

	FPlayerSaveData* GetPlayerData(APlayerState* PlayerState);
};
