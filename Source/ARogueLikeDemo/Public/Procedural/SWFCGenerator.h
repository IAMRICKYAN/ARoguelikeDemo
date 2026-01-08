// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SWFCGenerator.generated.h"

/**
 * 瓦片类型枚举
 */
UENUM(BlueprintType)
enum class ETileType : uint8
{
	Floor	UMETA(DisplayName = "Floor"),
	Wall	UMETA(DisplayName = "Wall")
};

/**
 * 方向枚举
 */
UENUM()
enum class EDirection : uint8
{
	North	UMETA(DisplayName = "North"),  // +X
	South	UMETA(DisplayName = "South"),  // -X
	East	UMETA(DisplayName = "East"),   // +Z
	West	UMETA(DisplayName = "West")    // -Z
};

/**
 * 波函数坍缩生成器
 * 用于生成随机地图
 */
UCLASS(BlueprintType)
class AROGUELIKEDEMO_API USWFCGenerator : public UObject
{
	GENERATED_BODY()

public:
	USWFCGenerator();

	/**
	 * 生成地图
	 * @param World 世界上下文
	 * @param MapWidth 地图宽度（格子数）
	 * @param MapHeight 地图高度（格子数）
	 * @param TileSize 每个瓦片的大小（单位：厘米）
	 * @param FloorTileClass Floor瓦片的蓝图类
	 * @param WallTileClass Wall瓦片的蓝图类
	 * @return 是否生成成功
	 */
	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool GenerateMap(UWorld* World, int32 MapWidth, int32 MapHeight, float TileSize, 
		TSubclassOf<AActor> FloorTileClass, TSubclassOf<AActor> WallTileClass);

	/**
	 * 获取生成的地图数据（用于后续Actor放置）
	 * @param OutFloorPositions 输出所有Floor瓦片的位置
	 */
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void GetFloorPositions(TArray<FVector>& OutFloorPositions) const;

	/**
	 * 检查指定位置是否为Floor（可通行）
	 */
	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool IsFloorAtPosition(const FVector& WorldPosition) const;

private:
	// 地图数据：存储每个格子的瓦片类型
	TArray<TArray<ETileType>> MapData;
	
	// 地图尺寸
	int32 Width;
	int32 Height;
	float TileSize;
	
	// 世界位置偏移（地图中心点）
	FVector MapOrigin;

	// 已生成的Actor引用（可选，用于清理）
	UPROPERTY()
	TArray<AActor*> SpawnedTiles;

	/**
	 * WFC算法核心：初始化所有格子为叠加态
	 */
	void InitializeSuperposition();

	/**
	 * WFC算法核心：观察（选择熵最低的格子并坍缩）
	 * @return 是否还有未坍缩的格子
	 */
	bool Observe();

	/**
	 * WFC算法核心：传播约束
	 */
	void Propagate();

	/**
	 * 计算格子的熵（可能性的数量）
	 */
	int32 CalculateEntropy(int32 X, int32 Y) const;

	/**
	 * 获取指定方向相邻格子的坐标
	 */
	FIntPoint GetNeighborCoordinate(int32 X, int32 Y, EDirection Direction) const;

	/**
	 * 检查两个瓦片类型是否可以在指定方向连接
	 */
	bool CanConnect(ETileType TileA, ETileType TileB, EDirection Direction) const;

	/**
	 * 获取指定格子所有可能的瓦片类型
	 */
	TArray<ETileType> GetPossibleTiles(int32 X, int32 Y) const;

	/**
	 * 在世界中生成实际的Actor
	 */
	void SpawnTilesInWorld(UWorld* World, TSubclassOf<AActor> FloorTileClass, TSubclassOf<AActor> WallTileClass);

	/**
	 * 将网格坐标转换为世界坐标
	 */
	FVector GridToWorldPosition(int32 X, int32 Y) const;

	/**
	 * 将世界坐标转换为网格坐标
	 */
	FIntPoint WorldToGridPosition(const FVector& WorldPosition) const;
};
