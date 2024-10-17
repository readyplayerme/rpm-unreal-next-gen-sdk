#pragma once
#include "CoreMinimal.h"
#include "Api/Common/Models/ApiResponse.h"
#include "CreateUserResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCreateUserResponseBody
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "_id") )
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "name"))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "email"))
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "token"))
	FString Token;

};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCreateUserResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "data"))
	FCreateUserResponseBody Data;
	
};