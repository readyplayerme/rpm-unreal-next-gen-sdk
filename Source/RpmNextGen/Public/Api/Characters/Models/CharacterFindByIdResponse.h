#pragma once

#include "CoreMinimal.h"
#include "RpmCharacter.h"
#include "Api/Common/Models/ApiResponse.h"
#include "CharacterFindByIdResponse.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCharacterFindByIdResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "data"))
	FRpmCharacter Data;
};
