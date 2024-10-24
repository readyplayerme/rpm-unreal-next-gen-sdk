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
}

void FAuthApi::RefreshToken(const FRefreshTokenRequest& Request, FOnRefreshTokenResponse OnComplete)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/refresh"), *RpmSettings->ApiBaseAuthUrl);
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	SendRequestWithAuth<FRefreshTokenResponse>(ApiRequest, [OnComplete](TSharedPtr<FRefreshTokenResponse> Response, bool bWasSuccessful)
	{
		OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	});
}

void FAuthApi::SendLoginCode(const FSendLoginCodeRequest& Request, FOnSendLoginCodeResponse OnComplete)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/auth/request-login-code"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	// TODO confirm this works since this request has no return data
	SendRequestWithAuth<FApiResponse>(ApiRequest, [OnComplete](TSharedPtr<FApiResponse> Response, bool bWasSuccessful)
	{
		OnComplete.ExecuteIfBound(bWasSuccessful && Response.IsValid());
	});
}

void FAuthApi::LoginWithCode(const FLoginWithCodeRequest& Request, FOnLoginWithCodeResponse OnComplete)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/auth/login"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	SendRequestWithAuth<FLoginWithCodeResponse>(ApiRequest, [OnComplete](TSharedPtr<FLoginWithCodeResponse> Response, bool bWasSuccessful)
	{
		OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	});
}

void FAuthApi::CreateUser(const FCreateUserRequest& Request, FOnCreateUserResponse OnComplete)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = FString::Printf(TEXT("%s/v1/users"), *RpmSettings->GetApiBaseUrl());
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(Request);
	SendRequestWithAuth<FCreateUserResponse>(ApiRequest, [OnComplete](TSharedPtr<FCreateUserResponse> Response, bool bWasSuccessful)
	{
		OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	});
}
