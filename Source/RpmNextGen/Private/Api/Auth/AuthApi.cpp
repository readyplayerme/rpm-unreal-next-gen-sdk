#include "Api/Auth/AuthApi.h"

#include "RpmNextGen.h"
#include "Interfaces/IHttpResponse.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FAuthApi::FAuthApi()
{
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	ApiUrl = FString::Printf(TEXT("%s/refresh"), *RpmSettings->ApiBaseAuthUrl);
	OnRequestComplete.BindRaw(this, &FAuthApi::OnProcessComplete);
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

void FAuthApi::OnProcessComplete(const FApiRequest& ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(bWasSuccessful, Response.IsValid() && Response->GetResponseCode() == 200)
	{
		FString Data = Response->GetContentAsString();
	
		FRefreshTokenResponse TokenResponse;
		if (!Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &TokenResponse, 0, 0))
		{
			OnRefreshTokenResponse.ExecuteIfBound(TokenResponse, true);
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Refresh token success"));
			return;
		}
	}
    UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to refresh token"));
	OnRefreshTokenResponse.ExecuteIfBound(FRefreshTokenResponse(), false);
}
