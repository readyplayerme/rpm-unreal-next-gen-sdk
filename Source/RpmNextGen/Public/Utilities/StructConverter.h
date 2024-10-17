#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"

namespace RpmJsonConverter
{
	template <typename StructType>
	bool StructToJsonString(const StructType& Struct, FString& OutJsonString)
	{
		return FJsonObjectConverter::UStructToJsonObjectString(Struct, OutJsonString);
	}

	template <typename StructType>
	bool StructFromJsonString(const FString& JsonString, StructType& OutStruct)
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &OutStruct, 0, 0);
	}
}
