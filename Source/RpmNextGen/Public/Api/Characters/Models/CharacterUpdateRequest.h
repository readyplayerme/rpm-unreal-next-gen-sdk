#pragma once

#include "CoreMinimal.h"
#include "CharacterUpdateRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterUpdateRequestBody
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "assets"))
	TMap<FString, FString> Assets;
};


USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterUpdateRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "applicationId"))
	FString Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FCharacterUpdateRequestBody Payload;
};
