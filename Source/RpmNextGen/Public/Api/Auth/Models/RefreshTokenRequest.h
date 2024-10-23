#pragma once

#include "CoreMinimal.h"
#include "RefreshTokenRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRefreshTokenRequestBody
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString RefreshToken;
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRefreshTokenRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FRefreshTokenRequestBody Data;
};
