#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/ScopeExit.h"
#include "Models/ApiRequest.h"

class FHttpModule;


class RPMNEXTGEN_API FWebApi
{
public:
	DECLARE_DELEGATE_ThreeParams(FOnRequestComplete, const FApiRequest&, FHttpResponsePtr, bool);
	
	FOnRequestComplete OnRequestComplete;
	
	FWebApi();
	virtual ~FWebApi();
	
	void DispatchRaw( const FApiRequest& Data);
protected:	
	FHttpModule* Http;
	TSharedPtr<IHttpRequest> CurrentRequest;

	
	FString BuildQueryString(const TMap<FString, FString>& QueryParams);
	
	template <typename T>
	FString ConvertToJsonString(const T& Data);
	
	virtual void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FApiRequest* ApiRequest);
};

template <typename T>
FString FWebApi::ConvertToJsonString(const T& Data)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	return JsonString;
}