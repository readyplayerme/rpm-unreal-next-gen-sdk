﻿#pragma once

#include "CoreMinimal.h"
#include "CreateUserRequest.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCreateUserRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "applicationId"))
	FString ApplicationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|Auth|Request", meta = (json = "requestToken"))
	bool RequestToken = true;
};
