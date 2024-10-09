#pragma once

#include "CoreMinimal.h"
#include "Pagination.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FPagination
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "totalDocs"))
	int TotalDocs = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "limit"))
	int Limit = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "totalPages"))
	int TotalPages = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "page"))
	int Page = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "pagingCounter"))
	int PagingCounter = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "hasPrevPage"))
	bool HasPrevPage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "hasNextPage"))
	bool HasNextPage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "prevPage"))
	int PrevPage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "nextPage"))
	int NextPage = 0;
};
