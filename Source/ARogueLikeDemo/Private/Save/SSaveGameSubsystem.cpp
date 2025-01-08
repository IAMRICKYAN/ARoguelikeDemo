// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/SSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "SGameplayInterface.h"
#include "SPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Save/SSaveGame.h"
#include "Save/SSaveGameSettings.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SSaveGameSubsystem)



void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const USSaveGameSettings* SGSettings = GetDefault<USSaveGameSettings>();
	//访问默认值从 DefaultGame.ini
	CurrentSlotName = SGSettings->SaveSlotName;
	//确保它已加载到内存中。Get（） 仅在之前已在代码中的其他位置加载时解析
	UDataTable* DummyTable = SGSettings->DummyTablePath.LoadSynchronous();
	//DummyTable->GetAllRows（） // 我们不需要这个表格做任何事情，只是一个内容引用示例
}

// 当玩家开始时，我们加载其状态
void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

// 当玩家开始时，我们检查其位置
bool USSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return false;
	}
	
	APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	if (PS == nullptr)
	{
		return false;
	}
	
	if (APawn* MyPawn = PS->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform)
		{		
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);
			// PlayerState owner is a (Player)Controller
			AController* PC = Cast<AController>(PS->GetOwner());
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			PC->SetControlRotation(FoundData->Rotation);
			
			return true;
		}
	}
	return false;
}

void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	//无视空字符串
	if (NewSlotName.Len() == 0)
	{
		return;
	}
	
	CurrentSlotName = NewSlotName;
}

void USSaveGameSubsystem::WriteSaveGame()
{
	//Clear 数组，可能包含来自以前加载的 SaveGame 的数据
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActors.Empty();
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		//保存失败时发出警告？
		return;
	}
	
	//迭代所有玩家状态，我们还没有合适的 ID 来匹配（需要 Steam 或 EOS）
	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GS->PlayerArray[i]);
		if (PS)
		{
			// 保存玩家状态任何状态
			PS->SavePlayerState(CurrentSaveGame);
			break; // 此时仅限单人游戏
		}
	}
	// 迭代整个 actor 世界
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// 只对我们的 'Gameplay actors' 感兴趣，跳过正在销毁的 actor
		// 注意： 您可以改为为要保存的 Actor 使用专用的 SavableObject 接口，而不是重复使用 GameplayInterface
		if (!IsValid(Actor) || !Actor->Implements<USGameplayInterface>())
		{
			continue;
		}
		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();
		
		// 传递数组以填充来自 Actor 的数据
		FMemoryWriter MemWriter(ActorData.ByteData);

		// 创建一个内存写入器，以写入 Actor 的数据
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		//仅查找使用 UPROPERTY（SaveGame）的变量
		Ar.ArIsSaveGame = true;
		//将 Actor 的 SaveGame UPROPERTIES 转换为二进制数组
		Actor->Serialize(Ar);
		CurrentSaveGame->SavedActors.Add(ActorData);
	}
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);
	OnSaveGameWritten.Broadcast(CurrentSaveGame);

	
}

void USSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	//如果指定，请先更新插槽名称，否则保留默认名称
	SetSlotName(InSlotName);
	
	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));
		//迭代整个 actor 世界
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// 只对gameplay actors感兴趣
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}
			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetFName())
				{
					Actor->SetActorTransform(ActorData.Transform);
					FMemoryReader MemReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					//将二进制数组转换回 actor 的变量
					Actor->Serialize(Ar);
					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					break;
				}
			}
		}
		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}
}



