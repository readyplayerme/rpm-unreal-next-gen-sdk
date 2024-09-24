#pragma once

UENUM(BlueprintType)
enum class ECacheStrategy : uint8
{
	None UMETA(DisplayName = "No Caching"),
	UseCache UMETA(DisplayName = "Cache on Disk"),
	UseMemory UMETA(DisplayName = "Cache in Memory")
};

UENUM(BlueprintType)
enum class ELoadingStrategy : uint8
{
	ApiOnly UMETA(DisplayName = "API only"),
	CacheFirst UMETA(DisplayName = "Cache first"),
	FallbackToCache UMETA(DisplayName = "Fallback to Cache")
};