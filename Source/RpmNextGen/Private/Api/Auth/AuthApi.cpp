#include "Api/Auth/AuthApi.h"
#include "Interfaces/IHttpResponse.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FAuthApi::FAuthApi()
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	ApiUrl = FString::Printf(TEXT("%s/refresh"), *Settings->ApiBaseAuthUrl);
}

void FAuthApi::RefreshToken(const FRefreshTokenRequest& Request)
{
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = ApiUrl;
	ApiRequest.Method = POST;
	ApiRequest.Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest.Payload = Request.ToJsonString();
	DispatchRaw(ApiRequest);
}

void FAuthApi::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString data = Response->GetContentAsString();
	
	FRefreshTokenResponse TokenResponse;
	if (bWasSuccessful && !data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(data, &TokenResponse, 0, 0))
	{
		OnRefreshTokenResponse.ExecuteIfBound(TokenResponse, true);
		return;
	}
	OnRefreshTokenResponse.ExecuteIfBound(FRefreshTokenResponse(), false);
}
