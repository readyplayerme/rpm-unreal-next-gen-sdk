#include "Api/Characters/CharacterApi.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Api/Characters/Models/CharacterFindByIdRequest.h"
#include "Api/Characters/Models/CharacterPreviewRequest.h"
#include "Api/Characters/Models/CharacterUpdateRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FCharacterApi::FCharacterApi()
{
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	BaseUrl = FString::Printf(TEXT("%s/v1/characters"), *RpmSettings->GetApiBaseUrl());
	Http = &FHttpModule::Get();
	SetAuthenticationStrategy(nullptr);
	if(!RpmSettings->ApiKey.IsEmpty() && RpmSettings->ApiProxyUrl.IsEmpty())
	{
		SetAuthenticationStrategy(new FApiKeyAuthStrategy());
	}
}

FCharacterApi::~FCharacterApi()
{
}

void FCharacterApi::CreateAsync(const FCharacterCreateRequest& Request)
{
	AssetByType.Append(Request.Data.Assets);
	FApiRequest ApiRequest;
	ApiRequest.Url = FString::Printf(TEXT("%s"), *BaseUrl);
	ApiRequest.Method = POST;
	ApiRequest.Payload = ConvertToJsonString(Request);
	ApiRequest.Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	DispatchRawWithAuth(ApiRequest);
}

void FCharacterApi::UpdateAsync(const FCharacterUpdateRequest& Request)
{
	AssetByType.Append(Request.Payload.Assets);
	FApiRequest ApiRequest;
	ApiRequest.Url = FString::Printf(TEXT("%s/%s"), *BaseUrl, *Request.Id);
	ApiRequest.Method = PATCH;
	ApiRequest.Payload = ConvertToJsonString(Request);
	ApiRequest.Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	DispatchRawWithAuth(ApiRequest);
}

void FCharacterApi::FindByIdAsync(const FCharacterFindByIdRequest& Request)
{
	FApiRequest ApiRequest;
	ApiRequest.Url = FString::Printf(TEXT("%s/%s"), *BaseUrl, *Request.Id);
	ApiRequest.Method = GET;
	ApiRequest.Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	DispatchRawWithAuth(ApiRequest);
}

FString FCharacterApi::GeneratePreviewUrl(const FCharacterPreviewRequest& Request)
{
	FString QueryString = BuildQueryString(Request.Params.Assets);
	FString url = FString::Printf(TEXT("%s/%s/preview%s"), *BaseUrl, *Request.Id, *QueryString);
	return url;
}

void FCharacterApi::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FWebApiWithAuth::OnProcessResponse(Request, Response, bWasSuccessful);
	bool bSuccess = bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode());
	if (Response->GetResponseCode() == 401)
	{
		UE_LOG(LogReadyPlayerMe, Error,TEXT("The request to the character API failed with a 401 response code. Please ensure that your API Key or proxy is correctly configured."));
		return;
	}
	
	const FString Verb = Request->GetVerb();
	if (Verb == "POST")
	{
		FCharacterCreateResponse CharacterCreateResponse;
		bSuccess = bSuccess && FJsonObjectConverter::JsonObjectStringToUStruct(
			Response->GetContentAsString(), &CharacterCreateResponse, 0, 0);
		OnCharacterCreateResponse.ExecuteIfBound(CharacterCreateResponse, bSuccess);
	}
	else if (Verb == "PATCH")
	{
		FCharacterUpdateResponse CharacterUpdateResponse;
		bSuccess = bSuccess && FJsonObjectConverter::JsonObjectStringToUStruct(
			Response->GetContentAsString(), &CharacterUpdateResponse, 0, 0);
		OnCharacterUpdateResponse.ExecuteIfBound(CharacterUpdateResponse, bSuccess);
	}
	else if (Verb == "GET")
	{
		FCharacterFindByIdResponse CharacterFindByIdResponse;
		bSuccess = bSuccess && FJsonObjectConverter::JsonObjectStringToUStruct(
			Response->GetContentAsString(), &CharacterFindByIdResponse, 0, 0);
		OnCharacterFindResponse.ExecuteIfBound(CharacterFindByIdResponse, bSuccess);
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Unhandled verb"));
	}
}

