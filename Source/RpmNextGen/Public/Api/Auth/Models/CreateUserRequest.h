#pragma once

#include "CoreMinimal.h"
#include "CreateUserRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCreateUserRequestBody
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "applicationId"))
	FString ApplicationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "requestToken"))
	bool RequestToken = true;

	FCreateUserRequestBody()
	{
		ApplicationId = TEXT("");
		RequestToken = true;
	}
	FCreateUserRequestBody(const FString& InApplicationId, const bool InRequestToken = true)
	{
		ApplicationId = InApplicationId;
		RequestToken = InRequestToken;
	}
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCreateUserRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "data"))
	FCreateUserRequestBody Data;

	FCreateUserRequest()
	{
		Data = FCreateUserRequestBody();
	}
	
	FCreateUserRequest(const FString& InApplicationId, const bool InRequestToken = true)
	{
		Data = FCreateUserRequestBody(InApplicationId, InRequestToken);
	}
};
