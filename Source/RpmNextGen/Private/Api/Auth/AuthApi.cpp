#include "Api/Auth/AuthApi.h"
#include "RpmNextGen.h"
#include "Api/Auth/Models/CreateUserRequest.h"
#include "Api/Auth/Models/CreateUserResponse.h"
#include "Api/Auth/Models/LoginWithCodeRequest.h"
#include "Api/Auth/Models/LoginWithCodeResponse.h"
#include "Interfaces/IHttpResponse.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"
#include "Api/Auth/Models/SendLoginCodeRequest.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Api/Auth/Models/SendLoginCodeResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FAuthApi::FAuthApi()
{
	RpmSettings = GetDefault<URpmDeveloperSettings>();
	
	OnRequestComplete.BindRaw(this, &FAuthApi::OnProcessComplete);
}

void FAuthApi::RefreshToken(const FRefreshTokenRequest& Request)
{
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/refresh"), *RpmSettings->ApiBaseAuthUrl);
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	DispatchRaw(ApiRequest);
}

void FAuthApi::SendLoginCode(const FSendLoginCodeRequest& Request)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/auth/request-login-code"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	DispatchRaw(ApiRequest);
}

void FAuthApi::LoginWithCode(const FLoginWithCodeRequest& Request)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/auth/login"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
}

void FAuthApi::CreateUser(const FCreateUserRequest& Request)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/users"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
}

void FAuthApi::OnProcessComplete(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ApiRequest.IsValid())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Invalid ApiRequest in FAuthApi::OnProcessComplete."));
		return;
	}

	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		FString Data = Response->GetContentAsString();
		FRefreshTokenResponse TokenResponse;

		if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &TokenResponse, 0, 0))
		{
			if (OnRefreshTokenResponse.IsBound())
			{
				OnRefreshTokenResponse.ExecuteIfBound(ApiRequest, TokenResponse, true);
			}
			return;
		}

		FSendLoginCodeResponse SendLoginCodeRequest;
		if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &SendLoginCodeRequest, 0, 0))
		{
			OnSendLoginCodeResponse.ExecuteIfBound(ApiRequest, SendLoginCodeRequest, true);
			return;
		}

		FLoginWithCodeResponse LoginWithCodeResponse;
		if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &LoginWithCodeResponse, 0, 0))
		{
			OnLoginWithCodeResponse.ExecuteIfBound(ApiRequest, LoginWithCodeResponse, true);
			return;
		}

		FCreateUserResponse CreateUserResponse;
		if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &CreateUserResponse, 0, 0))
		{
			OnCreateUserResponse.ExecuteIfBound(ApiRequest, CreateUserResponse, true);
			return;
		}
	}

	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to refresh token"));
	OnRefreshTokenResponse.ExecuteIfBound(ApiRequest, FRefreshTokenResponse(), false);
}
