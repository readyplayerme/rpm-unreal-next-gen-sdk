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
	if(!RpmSettings->ApiKey.IsEmpty() && RpmSettings->ApiProxyUrl.IsEmpty())
	{
		SetAuthenticationStrategy(MakeShared<FApiKeyAuthStrategy>());
	}
}

FCharacterApi::~FCharacterApi()
{
}

void FCharacterApi::CreateAsync(TSharedPtr<FCharacterCreateRequest> Request, FOnCharacterCreateResponse OnComplete)
{
	AssetByType.Append(Request->Data.Assets);
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s"), *BaseUrl);
	ApiRequest->Method = POST;
	ApiRequest->Payload = ConvertToJsonString(Request);
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	SendRequestWithAuth<FCharacterCreateResponse>(ApiRequest, [OnComplete](TSharedPtr<FCharacterCreateResponse> Response, bool bWasSuccessful)
	 {
		//UE_LOG(LogReadyPlayerMe, Warning, TEXT("Character CREATE request failed."));
		 OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	 });
}

void FCharacterApi::UpdateAsync(TSharedPtr<FCharacterUpdateRequest> Request, FOnCharacterUpdatResponse OnComplete)
{
	AssetByType.Append(Request->Payload.Assets);
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/%s"), *BaseUrl, *Request->Id);
	ApiRequest->Method = PATCH;
	ApiRequest->Payload = ConvertToJsonString(Request);
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	SendRequestWithAuth<FCharacterUpdateResponse>(ApiRequest, [OnComplete](TSharedPtr<FCharacterUpdateResponse> Response, bool bWasSuccessful)
	{
		//UE_LOG(LogReadyPlayerMe, Warning, TEXT("Character UPDATE request failed."));
		OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	});
}

void FCharacterApi::FindByIdAsync(TSharedPtr<FCharacterFindByIdRequest> Request, FOnCharacterFindResponse OnComplete)
{
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/%s"), *BaseUrl, *Request->Id);
	ApiRequest->Method = GET;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	SendRequestWithAuth<FCharacterFindByIdResponse>(ApiRequest, [OnComplete](TSharedPtr<FCharacterFindByIdResponse> Response, bool bWasSuccessful)
	{
		//UE_LOG(LogReadyPlayerMe, Warning, TEXT("Character FindById request failed."));
		OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	});
}

FString FCharacterApi::GeneratePreviewUrl(const FCharacterPreviewRequest& Request)
{
	FString QueryString = BuildQueryString(Request.Params.Assets);
	FString url = FString::Printf(TEXT("%s/%s/preview%s"), *BaseUrl, *Request.Id, *QueryString);
	return url;
}

// TODO re-add 401 check before removing this (as a reminder )
// void FCharacterApi::HandleCharacterResponse(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
// {
// 	const FString Verb = ApiRequest->GetVerb();
// 	bool bSuccess = bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode());
// 	if (Response.IsValid() && Response->GetResponseCode() == 401)
// 	{
// 		UE_LOG(LogReadyPlayerMe, Error,TEXT("The request to the character API failed with a 401 response code. Please ensure that your API Key or proxy is correctly configured."));
// 		bSuccess = false;
// 	}
// 	switch (ApiRequest->Method)
// 	{
// 		case POST:
// 			HandleCharacterCreateResponse(Response, bSuccess);
// 			break;
// 		case PATCH:
// 			HandleUpdateResponse( Response, bSuccess);
// 			break;
// 		case GET: 
// 			HandleFindResponse(Response, bSuccess);
// 			break;
// 		default:
// 			break;
// 	}
//}





