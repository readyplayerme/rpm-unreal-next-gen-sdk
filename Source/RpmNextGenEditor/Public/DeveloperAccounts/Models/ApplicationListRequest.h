#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/ApiRequest.h"
#include "ApplicationListRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FApplicationListRequest : public FApiRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
	TMap<FString, FString> Params;
};
