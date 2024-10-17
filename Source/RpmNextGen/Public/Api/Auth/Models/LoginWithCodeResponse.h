#pragma once

#include "CoreMinimal.h"
#include "Api/Common/Models/ApiResponse.h"
#include "LoginWithCodeResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FLoginWithCodeResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "_id"))
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "email"))
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "name"))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "token"))
	FString Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Response", meta = (json = "refreshToken"))
	FString RefreshToken;
	
};
