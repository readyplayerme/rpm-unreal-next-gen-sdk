#pragma once

#include "CoreMinimal.h"
#include "RpmNextGen.h"
#include "Utilities/StructConverter.h"
#include "LoginWithCodeRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FLoginWithCodeRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "code"))
	FString Code;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "appId"))
	FString ApplicationId;

	FString ToJsonString() const
	{
		FString OutputString;
		if(RpmJsonConverter::StructToJsonString( *this, OutputString ))
		{
			return OutputString;
		}
		UE_LOG(LogReadyPlayerMe, Error, TEXT("FLoginWithCodeRequest::ToJsonString - Failed to serialize struct to json string") );
		return OutputString;
	}
};
