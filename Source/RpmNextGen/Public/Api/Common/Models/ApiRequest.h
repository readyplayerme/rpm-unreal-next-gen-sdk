#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "ApiRequest.generated.h"

UENUM(BlueprintType)
enum ERequestMethod { 	
	GET,
	POST,
	PUT,
	DELETE,
	PATCH
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FApiRequest
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Url;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TEnumAsByte<ERequestMethod> Method = GET;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TMap<FString, FString> Headers;

	TMap<FString, FString> QueryParams;

	FString Payload;

	FString GetVerb() const
	{
		switch (Method)
		{
		case GET: default:
			return TEXT("GET");
		case POST:
			return TEXT("POST");
		case PUT:
			return TEXT("PUT");
		case PATCH:
			return TEXT("PATCH");
		case DELETE:
			return TEXT("DELETE");
		}
	}

	bool IsValid() const
	{
		return !Url.IsEmpty();
	}
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FApiRequestBody
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "data"))
	FString Data;
};