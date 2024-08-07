#pragma once

#include "CoreMinimal.h"
#include "ApiResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FApiResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	bool IsSuccess = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	int64 Status = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Error;
};