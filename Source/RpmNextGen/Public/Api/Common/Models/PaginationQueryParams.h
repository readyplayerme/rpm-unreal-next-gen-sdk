#pragma once

#include "CoreMinimal.h"
#include "PaginationQueryParams.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FPaginationQueryParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "limit"))
	int Limit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "page"))
	int Page;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "order"))
	FString Order;
};
