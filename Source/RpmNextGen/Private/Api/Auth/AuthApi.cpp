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
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = ApiUrl;
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = Request.ToJsonString();
	DispatchRaw(ApiRequest);
}

void FAuthApi::OnProcessComplete(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ApiRequest.IsValid())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Invalid ApiRequest in OnProcessComplete."));
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
	}

	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to refresh token"));
	if (OnRefreshTokenResponse.IsBound())
	{
		OnRefreshTokenResponse.ExecuteIfBound(ApiRequest, FRefreshTokenResponse(), false);
	}
}
