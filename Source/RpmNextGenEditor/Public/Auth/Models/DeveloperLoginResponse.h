#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Api/Common/Models/ApiResponse.h"
#include "RpmNextGen.h"
#include "DeveloperLoginResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FDeveloperLoginResponseBody : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me",  meta = (JsonName = "token"))
	FString Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "refreshToken"))
	FString RefreshToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "name"))
	FString Name;
	
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FDeveloperLoginResponse : public FApiResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "data"))
	FDeveloperLoginResponseBody Data;

	FString ToJsonString() const
	{
		FString OutputString;
		FJsonObjectConverter::UStructToJsonObjectString(*this, OutputString);
		return OutputString;
	}

	static bool FromJsonObject(const TSharedPtr<FJsonObject>& JsonObject, FDeveloperLoginResponse& OutObject)
	{
		if (JsonObject.IsValid())
		{
		return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StaticStruct(), &OutObject, 0, 0);
		}
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("JsonObject Invalid"));
		return false;
	}
};
