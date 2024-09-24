#pragma once

#include "CoreMinimal.h"
#include "Api/Common/Models/ApiResponse.h"
#include "AssetTypeListResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetTypeListResponse : public FApiResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "data"))
	TArray<FString> Data;
};
