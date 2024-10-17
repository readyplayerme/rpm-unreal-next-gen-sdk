#pragma once

#include "CoreMinimal.h"
#include "RpmNextGen.h"
#include "Utilities/StructConverter.h"
#include "SendLoginCodeRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FSendLoginCodeRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "email"))
	FString Email;
	
	FString ToJsonString() const
	{
		FString OutputString;
		if(RpmJsonConverter::StructToJsonString(*this, OutputString ))
		{
			return OutputString;
		}
		UE_LOG(LogReadyPlayerMe, Error, TEXT("FSendLoginCodeRequest::ToJsonString - Failed to serialize struct to json string") );
		return OutputString;
	}
};
