#pragma once

#include "CoreMinimal.h"
#include "AssetTypeListRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetTypeListQueryParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "applicationId"))
	FString ApplicationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "type"))
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "excludeTypes"))
	FString ExcludeTypes;
	
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetTypeListRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FAssetTypeListQueryParams Params;

	// Default constructor
	FAssetTypeListRequest()
	{
	}

	// Constructor that accepts FAssetListQueryParams
	FAssetTypeListRequest(const FAssetTypeListQueryParams& InParams)
		: Params(InParams)
	{
	}
};
