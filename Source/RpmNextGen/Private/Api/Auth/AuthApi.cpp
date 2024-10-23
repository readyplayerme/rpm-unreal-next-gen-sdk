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
	DispatchRaw(ApiRequest);
}

void FAuthApi::CreateUser(const FCreateUserRequest& Request)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/users"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	DispatchRaw(ApiRequest);
}

void FAuthApi::OnProcessComplete(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ApiRequest.IsValid())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Invalid ApiRequest in FAuthApi::OnProcessComplete."));
		return;
	}

	EAuthRequestType RequestType;
	if (ApiRequest->Url.Contains(TEXT("refresh")))
	{
		RequestType = EAuthRequestType::RefreshToken;
	}
	else if (ApiRequest->Url.Contains(TEXT("/auth/request-login-code")))
	{
		RequestType = EAuthRequestType::SendLoginCode;
	}
	else if (ApiRequest->Url.Contains(TEXT("auth/login")))
	{
		RequestType = EAuthRequestType::LoginWithCode;
	}
	else if (ApiRequest->Url.Contains(TEXT("users")))
	{
		RequestType = EAuthRequestType::CreateUser;
	}

	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		FString Data = Response->GetContentAsString();

		switch (RequestType)
		{
		case EAuthRequestType::RefreshToken:
			{
				FRefreshTokenResponse TokenResponse;
				if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &TokenResponse, 0, 0))
				{
					if (OnRefreshTokenResponse.IsBound())
					{
						OnRefreshTokenResponse.ExecuteIfBound(ApiRequest, TokenResponse, true);
					}
					return;
				}
				break;
			}
		case EAuthRequestType::SendLoginCode:
			{
				if(Response->GetResponseCode() == 201)
				{
					
					OnSendLoginCodeResponse.ExecuteIfBound(ApiRequest, true);
					return;
				}
				break;
			}
		case EAuthRequestType::LoginWithCode:
			{
				FLoginWithCodeResponse LoginWithCodeResponse;
				if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &LoginWithCodeResponse, 0, 0))
				{
					OnLoginWithCodeResponse.ExecuteIfBound(ApiRequest, LoginWithCodeResponse, true);
					return;
				}
				break;
			}
		case EAuthRequestType::CreateUser:
			{
				FCreateUserResponse CreateUserResponse;
				if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &CreateUserResponse, 0, 0))
				{
					OnCreateUserResponse.ExecuteIfBound(ApiRequest, CreateUserResponse, true);
					return;
				}
				break;
			}
		}
	}

	switch (RequestType)
	{
	case EAuthRequestType::RefreshToken:
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to refresh token"));
			OnRefreshTokenResponse.ExecuteIfBound(ApiRequest, FRefreshTokenResponse(), false);
			break;
		}
	case EAuthRequestType::SendLoginCode:
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to send login code"));
			OnSendLoginCodeResponse.ExecuteIfBound(ApiRequest, false);
			break;
		}
	case EAuthRequestType::LoginWithCode:
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to login with code"));
			OnLoginWithCodeResponse.ExecuteIfBound(ApiRequest, FLoginWithCodeResponse(), false);
			break;
		}
	case EAuthRequestType::CreateUser:
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to create user"));
			OnCreateUserResponse.ExecuteIfBound(ApiRequest, FCreateUserResponse(), false);
			break;
		}
	}
}
