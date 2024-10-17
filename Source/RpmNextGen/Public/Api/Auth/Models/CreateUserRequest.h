#pragma once

#include "CoreMinimal.h"
#include "RpmNextGen.h"
#include "Utilities/StructConverter.h"
#include "CreateUserRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCreateUserRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "applicationId"))
	FString ApplicationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "requestToken"))
	bool RequestToken = true;

	FString ToJsonString() const
	{
		FString OutputString;
		if(RpmJsonConverter::StructToJsonString( *this, OutputString ))
		{
			return OutputString;
		}
		UE_LOG(LogReadyPlayerMe, Error, TEXT("FCreateUserRequest::ToJsonString - Failed to serialize struct to json string") );
		return OutputString;
	}
};
