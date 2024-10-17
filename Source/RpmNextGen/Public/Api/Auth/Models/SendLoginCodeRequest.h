#pragma once

#include "CoreMinimal.h"
#include "SendLoginCodeRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FSendLoginCodeRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "email"))
	FString Email;
};
