#include "Auth/DeveloperAuthApi.h"
#include "Auth/Models/DeveloperLoginRequest.h"
#include "Auth/Models/DeveloperLoginResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FDeveloperAuthApi::FDeveloperAuthApi()
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	ApiUrl = FString::Printf(TEXT("%s/login"), *Settings->ApiBaseAuthUrl);
	OnApiResponse.BindRaw(this, &FDeveloperAuthApi::HandleLoginResponse);
}

void FDeveloperAuthApi::HandleLoginResponse(FString JsonData, bool bIsSuccessful) const
{
	FDeveloperLoginResponse Response;
	if (bIsSuccessful && !JsonData.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(JsonData, &Response, 0, 0))
	{
		OnLoginResponse.ExecuteIfBound(Response, true);
		return;
	}
	OnLoginResponse.ExecuteIfBound(Response, bIsSuccessful);
}

void FDeveloperAuthApi::LoginWithEmail(FDeveloperLoginRequest Request)
{
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = ApiUrl;
	ApiRequest.Method = POST;
	ApiRequest.Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest.Payload = Request.ToJsonString();
	DispatchRaw(ApiRequest);
}
