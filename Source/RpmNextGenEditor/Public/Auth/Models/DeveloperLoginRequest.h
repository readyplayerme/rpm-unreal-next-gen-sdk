#pragma once

#include "CoreMinimal.h"
#include "DeveloperLoginRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FDeveloperLoginRequestBody
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString LoginId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Password;
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FDeveloperLoginRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Request")
	FDeveloperLoginRequestBody Data;
	
	FDeveloperLoginRequest() = default;
	FDeveloperLoginRequest(const FDeveloperLoginRequestBody& DeveloperLoginRequestBody);
	FDeveloperLoginRequest(const FString& Email, const FString& String)
	{
		Data.LoginId = Email;
		Data.Password = String;
	}
};
