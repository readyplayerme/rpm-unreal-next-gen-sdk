#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "AssetListResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetListResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "data"))
	TArray<FAsset> Data;
	bool bSuccess;
	int64 Status;
	FString Error;
};
