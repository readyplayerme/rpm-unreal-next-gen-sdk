﻿#pragma once

#include "CoreMinimal.h"
#include "Api/Common/Models/PaginationQueryParams.h"
#include "AssetListRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetListQueryParams : public FPaginationQueryParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "applicationId"))
	FString ApplicationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "type"))
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "excludeTypes"))
	FString ExcludeTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "characterModelAssetId"))
	FString CharacterModelAssetId;
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetListRequest 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FAssetListQueryParams Params;

	// Default constructor
	FAssetListRequest()
	{
	}

	// Constructor that accepts FAssetListQueryParams
	FAssetListRequest(const FAssetListQueryParams& InParams)
		: Params(InParams)
	{
	}

	FString BuildQueryString() const;
};

inline FString FAssetListRequest::BuildQueryString() const
{
	if (Params.ApplicationId.IsEmpty() && Params.Type.IsEmpty() && Params.ExcludeTypes.IsEmpty()) return FString();
	FString QueryString = TEXT("?");
	if (!Params.ApplicationId.IsEmpty())
	{
		QueryString += TEXT("applicationId=") + Params.ApplicationId + TEXT("&");
	}
	if (!Params.Type.IsEmpty())
	{
		QueryString += TEXT("type=") + Params.Type + TEXT("&");
	}
	if (!Params.ExcludeTypes.IsEmpty())
	{
		QueryString += TEXT("excludeTypes=") + Params.ExcludeTypes + TEXT("&");
	}
	if (!Params.CharacterModelAssetId.IsEmpty())
	{
		QueryString += TEXT("characterModelAssetId=") + Params.CharacterModelAssetId + TEXT("&");
	}
	QueryString.RemoveFromEnd(TEXT("&"));
	return QueryString;
}
