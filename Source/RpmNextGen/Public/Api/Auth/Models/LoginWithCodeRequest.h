#pragma once

#include "CoreMinimal.h"
#include "LoginWithCodeRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FLoginWithCodeRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "code"))
	FString Code;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "appId"))
	FString ApplicationId;
};
