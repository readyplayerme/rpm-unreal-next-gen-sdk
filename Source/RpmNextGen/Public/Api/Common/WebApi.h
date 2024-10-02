#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Api/Auth/ApiRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/ScopeExit.h"

class FHttpModule;


class RPMNEXTGEN_API FWebApi
{
public:
	DECLARE_DELEGATE_ThreeParams(FOnRequestComplete, FHttpRequestPtr, FHttpResponsePtr, bool);
	
	FOnRequestComplete OnRequestComplete;
	
	FWebApi();
	virtual ~FWebApi();
	
protected:	
	FHttpModule* Http;
	
	void DispatchRaw(
		const FApiRequest& Data
	);
	
	FString BuildQueryString(const TMap<FString, FString>& QueryParams);
	
	template <typename T>
	FString ConvertToJsonString(const T& Data);
	
	virtual void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};

template <typename T>
FString FWebApi::ConvertToJsonString(const T& Data)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	return JsonString;
}