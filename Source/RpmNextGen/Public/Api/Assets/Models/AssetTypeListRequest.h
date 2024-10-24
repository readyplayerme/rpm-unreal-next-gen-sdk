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


	FAssetTypeListRequest()
	{
	}
	
	FAssetTypeListRequest(const FAssetTypeListQueryParams& InParams)
		: Params(InParams)
	{
	}

	FString BuildQueryString() const;
};

inline FString FAssetTypeListRequest::BuildQueryString() const
{
	if (Params.ApplicationId.IsEmpty() && Params.Type.IsEmpty() && Params.ExcludeTypes.IsEmpty()) return FString();
	FString QueryString = TEXT("?");
	if (!Params.ApplicationId.IsEmpty())
	{
		QueryString += TEXT("applicationId=") + Params.ApplicationId + TEXT("&");
	}
	if (!Params.Type.IsEmpty())
	{
		const FString CleanType = Params.Type.Replace(TEXT(" "), TEXT("%20"));
		QueryString += TEXT("type=") + Params.Type + TEXT("&");
	}
	if (!Params.ExcludeTypes.IsEmpty())
	{
		QueryString += TEXT("excludeTypes=") + Params.ExcludeTypes + TEXT("&");
	}
	QueryString.RemoveFromEnd(TEXT("&"));
	return QueryString;
}