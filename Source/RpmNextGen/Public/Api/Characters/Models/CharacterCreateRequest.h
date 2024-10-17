#pragma once

#include "CoreMinimal.h"
#include "CharacterCreateRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterCreateRequestBody
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "applicationId"))
	FString ApplicationId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "assets"))
	TMap<FString, FString> Assets;
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterCreateRequest
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FCharacterCreateRequestBody Data;
};