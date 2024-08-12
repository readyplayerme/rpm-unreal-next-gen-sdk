#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Api/Common/Models/ApiResponse.h"
#include "RefreshTokenResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRefreshTokenResponseBody : public FApiResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response")
	FString Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response")
	FString RefreshToken;

	FString ToJsonString() const
	{
		FString OutputString;
		FJsonObjectConverter::UStructToJsonObjectString(*this, OutputString);
		return OutputString;
	}

	static bool FromJsonString(const FString& JsonString, FRefreshTokenResponseBody& OutStruct)
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &OutStruct, 0, 0);
	}
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRefreshTokenResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FRefreshTokenResponseBody Data;

	FString ToJsonString() const
	{
		FString OutputString;
		FJsonObjectConverter::UStructToJsonObjectString(*this, OutputString);
		return OutputString;
	}

	static bool FromJsonString(const FString& JsonString, FRefreshTokenResponse& OutStruct)
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &OutStruct, 0, 0);
	}
};


