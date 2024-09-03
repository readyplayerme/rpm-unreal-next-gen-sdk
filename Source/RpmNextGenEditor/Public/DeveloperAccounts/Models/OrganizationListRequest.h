#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/ApiRequest.h"
#include "OrganizationListRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FOrganizationListRequest : public FApiRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
	TMap<FString, FString> Params;
};
