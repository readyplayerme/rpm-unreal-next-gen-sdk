#pragma once

#include "CoreMinimal.h"
#include "CharacterPreviewRequest.generated.h"


USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterPreviewQueryParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "assets"))
	TMap<FString, FString> Assets;
};


USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterPreviewRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FCharacterPreviewQueryParams Params;
};
