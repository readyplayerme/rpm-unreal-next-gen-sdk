#pragma once

#include "CoreMinimal.h"
#include "RpmCharacter.h"
#include "Api/Common/Models/ApiResponse.h"
#include "CharacterCreateResponse.generated.h"

USTRUCT()
struct RPMNEXTGEN_API FCharacterCreateResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "data"))
	FRpmCharacter Data;

	FCharacterCreateResponse()
	{		
	}

	FCharacterCreateResponse(FRpmCharacter Data)
	{
		this->Data = Data;
	}

	bool IsValid() const
	{
		return !Data.Id.IsEmpty() && !Data.GlbUrl.IsEmpty();
	}
};
