#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "RpmNextGen.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ScopeExit.h"
#include "Models/ApiRequest.h"

class FHttpModule;


class RPMNEXTGEN_API FWebApi : public TSharedFromThis<FWebApi> 
{
public:	
	FWebApi();
	virtual ~FWebApi();
	
	template <typename T>
	void SendRequest(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<T>, bool)> OnResponse);

	void CancelAllRequests();
	
protected:	
	FHttpModule* HttpModule;
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> ActiveRequests;
	
	FString BuildQueryString(const TMap<FString, FString>& QueryParams);
	
	template <typename T>
	FString ConvertToJsonString(const T& Data);
	
};

template <typename T>
void FWebApi::SendRequest(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<T>, bool)> OnResponse)
{
	if (!ApiRequest.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid API request"));
		return;
	}

	FString FullUrl = ApiRequest->Url;
	if (ApiRequest->QueryParams.Num() > 0)
	{
		FullUrl += "?";
		for (const auto& QueryParam : ApiRequest->QueryParams)
		{
			FullUrl += FString::Printf(TEXT("%s=%s&"), *QueryParam.Key, *QueryParam.Value);
		}
		FullUrl.RemoveFromEnd("&");
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
	HttpRequest->SetURL(FullUrl);
	HttpRequest->SetVerb(ApiRequest->GetVerb());
	UE_LOG(LogTemp, Log, TEXT("Making request to %s"), *FullUrl);

	for (const auto& Header : ApiRequest->Headers)
	{
		HttpRequest->SetHeader(Header.Key, Header.Value);
	}

	if (ApiRequest->Method != GET && !ApiRequest->Payload.IsEmpty())
	{
		HttpRequest->SetContentAsString(ApiRequest->Payload);
	}

	ActiveRequests.Add(HttpRequest);

	TWeakPtr<FWebApi> WeakApiPtr  = AsShared();

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[WeakApiPtr , OnResponse, HttpRequest](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!WeakApiPtr .IsValid())
			{
				return;
			}

			WeakApiPtr.Pin()->ActiveRequests.Remove(HttpRequest);

			if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				TSharedPtr<T> ParsedResponse = MakeShareable(new T());
				FString ContentAsString = Response->GetContentAsString();
				FJsonObjectConverter::JsonObjectStringToUStruct(ContentAsString, ParsedResponse.Get(), 0, 0);

				OnResponse(ParsedResponse, true);
			}
			else
			{
				const FString ErrorMessage = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
				UE_LOG(LogReadyPlayerMe, Warning, TEXT("WebApi from URL %s request failed: %s"), *Request->GetURL(), *ErrorMessage);
				OnResponse(nullptr, false);
			}
		}
	);

	HttpRequest->ProcessRequest();
}

template <typename T>
FString FWebApi::ConvertToJsonString(const T& Data)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	return JsonString;
}