#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Api/Auth/ApiRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/ScopeExit.h"

class FHttpModule;
DECLARE_DELEGATE_TwoParams(FOnWebApiResponse, FString, bool);

class RPMNEXTGEN_API FWebApi
{
public:
	FWebApi();
	virtual ~FWebApi();
	
	FOnWebApiResponse OnApiResponse;
protected:	
	
	// template <typename TRequestBody>
	// void Dispatch(
	// 	const FApiRequest<TRequestBody>& Data
	// );

	void DispatchRaw(
		const FApiRequest& Data
	);
	
	void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FString BuildQueryString(const TMap<FString, FString>& QueryParams);
	
	template <typename T>
	FString ConvertToJsonString(const T& Data);

	FHttpModule* Http;
};

template <typename T>
FString FWebApi::ConvertToJsonString(const T& Data)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	return JsonString;
}