// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/SSaveGame.h"
#include "GameFramework/PlayerState.h"
#include "Online/CoreOnline.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SSaveGame)


FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	if (PlayerState == nullptr)
	{
		return nullptr;
	}
	// 在 PIE 时不会提供唯一 ID，因此我们在编辑器中测试时跳过该步骤。
	// UObjects 无法访问 UWorld，因此我们通过 PlayerState 获取它
	if (PlayerState->GetWorld()->IsPlayInEditor())
	{
		UE_LOG(LogTemp, Log, TEXT("During PIE we cannot use PlayerID to retrieve Saved Player data. Using first entry in array if available."));
		//检查 SavedPlayers 数组是否至少有一个元素。如果有，则返回指向第一个元素的指针。
		if (SavedPlayers.IsValidIndex(0))
		{
			return &SavedPlayers[0];
		}
		// 没有可用的已保存玩家数据
		return nullptr;
	}

	// 下面是如果在Steam中登录的话 根据SteamID进行匹配，我们使用IDE的话是不跑下方逻辑的
	FUniqueNetIdRepl PlayerID = PlayerState->GetUniqueId();
	//FString PlayerID = PlayerState->GetUniqueId().ToString();
	// 迭代数组并通过 PlayerID 进行匹配（例如，Steam 提供的唯一 ID）
	return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerID; });
}
