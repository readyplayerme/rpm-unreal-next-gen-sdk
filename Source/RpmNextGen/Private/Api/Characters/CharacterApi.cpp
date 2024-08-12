#include "Api/Characters/CharacterApi.h"
#include "HttpModule.h"
#include "Api/Characters/Models/CharacterFindByIdRequest.h"
#include "Api/Characters/Models/CharacterPreviewRequest.h"
#include "Api/Characters/Models/CharacterUpdateRequest.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FCharacterApi::FCharacterApi()
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	BaseUrl = FString::Printf(TEXT("%s/v1/characters"), *Settings->ApiBaseUrl) ;
	Http = &FHttpModule::Get();
}

FCharacterApi::~FCharacterApi()
{
}

void FCharacterApi::CreateAsync(const FCharacterCreateRequest& Request)
{
	FApiRequest ApiRequest;
	ApiRequest.Url = FString::Printf(TEXT("%s"), *BaseUrl);
	ApiRequest.Method = POST;
	ApiRequest.Payload = ConvertToJsonString(Request);
	DispatchRaw(ApiRequest, Create);
}

void FCharacterApi::UpdateAsync(const FCharacterUpdateRequest& Request)
{
	FApiRequest ApiRequest;
	ApiRequest.Url = FString::Printf(TEXT("%s/%s"), *BaseUrl, *Request.Id );
	ApiRequest.Method = PATCH;
	ApiRequest.Payload = ConvertToJsonString(Request);
	DispatchRaw(ApiRequest, Update);
}

void FCharacterApi::FindByIdAsync(const FCharacterFindByIdRequest& Request)
{
	FApiRequest ApiRequest;
	ApiRequest.Url = FString::Printf(TEXT("%s/%s"), *BaseUrl, *Request.Id );
	ApiRequest.Method = GET;
	DispatchRaw(ApiRequest, FindById);
}

FString FCharacterApi::GeneratePreviewUrl(const FCharacterPreviewRequest& Request)
{
	FString QueryString = BuildQueryString(Request.Params.Assets);
	FString url = FString::Printf(TEXT("%s/%s/preview%s"), *BaseUrl, *Request.Id, *QueryString);
	return url;
}

void FCharacterApi::DispatchRaw(const FApiRequest& Data, const ECharacterResponseType& ResponseType)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Data.Url);
	Request->SetVerb(Data.GetVerb());
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	//TODO move to auth strategy
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>(); 
	Request->SetHeader(TEXT("X-API-KEY"), Settings->ApiKey);
	UE_LOG(LogTemp, Warning, TEXT("Making request to Url: %s | Verb: %s with data %s"), *Data.Url, *Data.GetVerb(), *Data.Payload);
	for (const auto& Header : Data.Headers)
	{
		if(!Request->GetHeader(Header.Key).IsEmpty())
			continue;
		Request->SetHeader(Header.Key, Header.Value);
		
	}

	if (!Data.Payload.IsEmpty())
	{
		Request->SetContentAsString(Data.Payload);
	}
	Request->OnProcessRequestComplete().BindLambda([this, ResponseType](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunCallback"))
		OnProcessResponse(Request, Response, bWasSuccessful, ResponseType);
	});
	Request->ProcessRequest();
}

void FCharacterApi::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const ECharacterResponseType& ResponseType)
{
	bool success = bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode());
	FCharacterCreateResponse CharacterCreateResponse;
	FCharacterUpdateResponse CharacterUpdateResponse;
	FCharacterFindByIdResponse CharacterFindByIdResponse;
	UE_LOG(LogTemp, Warning, TEXT("Character API Response: %s. Response Code: %d"), *Response->GetContentAsString(), Response->GetResponseCode());

	switch (ResponseType)
	{
	case Create:
		success = success && FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &CharacterCreateResponse, 0, 0);
		OnCharacterCreateResponse.ExecuteIfBound(CharacterCreateResponse, success);
		break;
	case Update:
		success = success && FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &CharacterUpdateResponse, 0, 0);
		OnCharacterUpdateResponse.ExecuteIfBound(CharacterUpdateResponse, success);
		break;
	case FindById:
		success = success && FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &CharacterFindByIdResponse, 0, 0);
		OnCharacterFindResponse.ExecuteIfBound(CharacterFindByIdResponse, success);
		break;
			
	}
// 	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("WebApi Response Success: %s. Response Code: %d"), *Response->GetContentAsString(), Response->GetResponseCode());
// 		OnApiResponse.ExecuteIfBound(Response->GetContentAsString(), true);
// 		return;
// 	}
// 	FString ErrorMessage = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
// 	UE_LOG(LogTemp, Warning, TEXT("WebApi request failed: %s"), *ErrorMessage);
// 	OnApiResponse.ExecuteIfBound(Response->GetContentAsString(), false);
}

FString FCharacterApi::BuildQueryString(const TMap<FString, FString>& QueryParams)
{
	FString QueryString;
	if (QueryParams.Num() > 0)
	{
		QueryString.Append(TEXT("?"));
		for (const auto& Param : QueryParams)
		{
			QueryString.Append(FString::Printf(TEXT("%s=%s&"), *FGenericPlatformHttp::UrlEncode(Param.Key), *FGenericPlatformHttp::UrlEncode(Param.Value)));
		}
		QueryString.RemoveFromEnd(TEXT("&"));
	}
	return QueryString;
}
