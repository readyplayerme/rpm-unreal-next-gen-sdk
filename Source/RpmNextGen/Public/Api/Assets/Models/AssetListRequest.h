#pragma once

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
	TArray<FString> ExcludeTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "characterModelAssetId"))
	FString CharacterModelAssetId;
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetListRequest 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FAssetListQueryParams Params;

	FAssetListRequest()
	{
	}

	FAssetListRequest(const FAssetListQueryParams& InParams)
		: Params(InParams)
	{
	}

	TMap<FString, FString> BuildQueryMap() const;
};

inline TMap<FString, FString> FAssetListRequest::BuildQueryMap() const
{
	TMap<FString, FString> QueryMap;
	if (!Params.ApplicationId.IsEmpty())
	{
		QueryMap.Add(TEXT("applicationId"), Params.ApplicationId);
	}
	if (!Params.Type.IsEmpty())
	{
		const FString CleanType = Params.Type.Replace(TEXT(" "), TEXT("%20")); // Replace spaces with %20 to make it URL friendly
		QueryMap.Add(TEXT("type"), CleanType);
	}
	if (!Params.ExcludeTypes.IsEmpty())
	{
		if (Params.ExcludeTypes.Num() > 0)
		{
			FString ExcludeTypesString;
			for (int32 i = 0; i < Params.ExcludeTypes.Num(); i++)
			{
				ExcludeTypesString += Params.ExcludeTypes[i];
				// Add '&excludeTypes=' only if it's not the last element
				if (i < Params.ExcludeTypes.Num() - 1)
				{
					ExcludeTypesString += TEXT("&excludeTypes=");
				}
			}
			const FString CleanExcludeTypes = ExcludeTypesString.Replace(TEXT(" "), TEXT("%20")); // Replace spaces with %20 to make it URL friendly
			QueryMap.Add(TEXT("excludeTypes"), CleanExcludeTypes);
		}
	}
	if (!Params.CharacterModelAssetId.IsEmpty())
	{
		QueryMap.Add(TEXT("characterModelAssetId"), Params.CharacterModelAssetId);
	}
	if( Params.Limit > 0 )
	{
		QueryMap.Add(TEXT("limit"), FString::FromInt(Params.Limit));
	}
	if( Params.Page > 0 )
	{
		QueryMap.Add(TEXT("page"), FString::FromInt(Params.Page));
	}
	if( !Params.Order.IsEmpty() )
	{
		QueryMap.Add(TEXT("order"), Params.Order);
	}
	return QueryMap;
}
