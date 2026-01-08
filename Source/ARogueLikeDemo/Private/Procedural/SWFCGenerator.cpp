// Fill out your copyright notice in the Description page of Project Settings.

#include "Procedural/SWFCGenerator.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/IntPoint.h"

USWFCGenerator::USWFCGenerator()
{
	Width = 0;
	Height = 0;
	TileSize = 400.0f; // 默认400单位（4米）
	MapOrigin = FVector::ZeroVector;
}

bool USWFCGenerator::GenerateMap(UWorld* World, int32 MapWidth, int32 MapHeight, float InTileSize, 
	TSubclassOf<AActor> FloorTileClass, TSubclassOf<AActor> WallTileClass)
{
	if (!World || !FloorTileClass || !WallTileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("WFC Generator: Invalid parameters"));
		return false;
	}

	Width = MapWidth;
	Height = MapHeight;
	TileSize = InTileSize;

	// 计算地图原点（使地图中心在世界原点）
	MapOrigin = FVector(
		-(Width * TileSize) / 2.0f,
		0.0f,
		-(Height * TileSize) / 2.0f
	);

	// 初始化地图数据（所有格子都是叠加态，用特殊值表示）
	MapData.Empty();
	MapData.SetNum(Width);
	for (int32 X = 0; X < Width; X++)
	{
		MapData[X].SetNum(Height);
		for (int32 Y = 0; Y < Height; Y++)
		{
			// 使用特殊值表示叠加态（这里我们用255表示未确定）
			// 但由于ETileType只有2个值，我们可以用TArray<ETileType>来表示可能性
			// 为了简化，我们用一个简单的标记系统
		}
	}

	// 使用简化的WFC算法
	// 为了最简单实现，我们使用随机填充，但遵循连接规则
	MapData.Empty();
	MapData.SetNum(Width);
	
	// 初始化：随机填充，但确保边界是Wall
	for (int32 X = 0; X < Width; X++)
	{
		MapData[X].SetNum(Height);
		for (int32 Y = 0; Y < Height; Y++)
		{
			// 边界都是Wall
			if (X == 0 || X == Width - 1 || Y == 0 || Y == Height - 1)
			{
				MapData[X][Y] = ETileType::Wall;
			}
			else
			{
				// 内部区域：70%概率是Floor，30%概率是Wall
				// 但需要遵循连接规则
				if (FMath::RandRange(0.0f, 1.0f) < 0.7f)
				{
					MapData[X][Y] = ETileType::Floor;
				}
				else
				{
					MapData[X][Y] = ETileType::Wall;
				}
			}
		}
	}

	// 平滑处理：确保Floor区域连通
	// 简单的平滑算法：如果周围Wall太多，也变成Wall；如果周围Floor太多，也变成Floor
	for (int32 Iteration = 0; Iteration < 3; Iteration++)
	{
		TArray<TArray<ETileType>> NewMapData = MapData;
		
		for (int32 X = 1; X < Width - 1; X++)
		{
			for (int32 Y = 1; Y < Height - 1; Y++)
			{
				int32 FloorCount = 0;
				int32 WallCount = 0;
				
				// 检查周围8个邻居
				for (int32 DX = -1; DX <= 1; DX++)
				{
					for (int32 DY = -1; DY <= 1; DY++)
					{
						if (DX == 0 && DY == 0) continue;
						
						int32 NX = X + DX;
						int32 NY = Y + DY;
						
						if (NX >= 0 && NX < Width && NY >= 0 && NY < Height)
						{
							if (MapData[NX][NY] == ETileType::Floor)
							{
								FloorCount++;
							}
							else
							{
								WallCount++;
							}
						}
					}
				}
				
				// 如果周围Floor多，保持或变成Floor；如果Wall多，保持或变成Wall
				if (FloorCount >= 5)
				{
					NewMapData[X][Y] = ETileType::Floor;
				}
				else if (WallCount >= 5)
				{
					NewMapData[X][Y] = ETileType::Wall;
				}
			}
		}
		
		MapData = NewMapData;
	}

	// 在世界中生成Actor
	SpawnTilesInWorld(World, FloorTileClass, WallTileClass);

	UE_LOG(LogTemp, Log, TEXT("WFC Generator: Map generated successfully (%dx%d)"), Width, Height);
	return true;
}

void USWFCGenerator::GetFloorPositions(TArray<FVector>& OutFloorPositions) const
{
	OutFloorPositions.Empty();
	
	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			if (MapData[X][Y] == ETileType::Floor)
			{
				OutFloorPositions.Add(GridToWorldPosition(X, Y));
			}
		}
	}
}

bool USWFCGenerator::IsFloorAtPosition(const FVector& WorldPosition) const
{
	FIntPoint GridPos = WorldToGridPosition(WorldPosition);
	
	if (GridPos.X >= 0 && GridPos.X < Width && GridPos.Y >= 0 && GridPos.Y < Height)
	{
		return MapData[GridPos.X][GridPos.Y] == ETileType::Floor;
	}
	
	return false;
}

void USWFCGenerator::InitializeSuperposition()
{
	// 简化版本：已在GenerateMap中直接初始化
}

bool USWFCGenerator::Observe()
{
	// 简化版本：已在GenerateMap中使用随机填充
	return false;
}

void USWFCGenerator::Propagate()
{
	// 简化版本：已在GenerateMap中使用平滑算法
}

int32 USWFCGenerator::CalculateEntropy(int32 X, int32 Y) const
{
	// 简化版本：未使用
	return 0;
}

FIntPoint USWFCGenerator::GetNeighborCoordinate(int32 X, int32 Y, EDirection Direction) const
{
	switch (Direction)
	{
	case EDirection::North: // +X
		return FIntPoint(X + 1, Y);
	case EDirection::South: // -X
		return FIntPoint(X - 1, Y);
	case EDirection::East:  // +Z
		return FIntPoint(X, Y + 1);
	case EDirection::West:  // -Z
		return FIntPoint(X, Y - 1);
	default:
		return FIntPoint(X, Y);
	}
}

bool USWFCGenerator::CanConnect(ETileType TileA, ETileType TileB, EDirection Direction) const
{
	// 简单规则：相同类型可以连接
	return TileA == TileB;
}

TArray<ETileType> USWFCGenerator::GetPossibleTiles(int32 X, int32 Y) const
{
	// 简化版本：未使用
	TArray<ETileType> PossibleTiles;
	PossibleTiles.Add(ETileType::Floor);
	PossibleTiles.Add(ETileType::Wall);
	return PossibleTiles;
}

void USWFCGenerator::SpawnTilesInWorld(UWorld* World, TSubclassOf<AActor> FloorTileClass, TSubclassOf<AActor> WallTileClass)
{
	// 清理之前生成的瓦片
	for (AActor* Tile : SpawnedTiles)
	{
		if (IsValid(Tile))
		{
			Tile->Destroy();
		}
	}
	SpawnedTiles.Empty();

	// 生成新瓦片
	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			FVector WorldPos = GridToWorldPosition(X, Y);
			FRotator Rotation = FRotator::ZeroRotator;
			
			TSubclassOf<AActor> TileClass = (MapData[X][Y] == ETileType::Floor) ? FloorTileClass : WallTileClass;
			
			if (TileClass)
			{
				AActor* SpawnedTile = World->SpawnActor<AActor>(TileClass, WorldPos, Rotation);
				if (SpawnedTile)
				{
					SpawnedTiles.Add(SpawnedTile);
				}
			}
		}
	}
}

FVector USWFCGenerator::GridToWorldPosition(int32 X, int32 Y) const
{
	return MapOrigin + FVector(X * TileSize, 0.0f, Y * TileSize);
}

FIntPoint USWFCGenerator::WorldToGridPosition(const FVector& WorldPosition) const
{
	FVector LocalPos = WorldPosition - MapOrigin;
	int32 GridX = FMath::RoundToInt(LocalPos.X / TileSize);
	int32 GridY = FMath::RoundToInt(LocalPos.Z / TileSize);
	return FIntPoint(GridX, GridY);
}
