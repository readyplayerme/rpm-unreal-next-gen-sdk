#include "Auth/DeveloperAuthApi.h"
#include "Auth/Models/DeveloperLoginRequest.h"
#include "Auth/Models/DeveloperLoginResponse.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FDeveloperAuthApi::FDeveloperAuthApi()
{
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	ApiUrl = FString::Printf(TEXT("%s/login"), *RpmSettings->ApiBaseAuthUrl);
}

void FDeveloperAuthApi::LoginWithEmail(TSharedPtr<FDeveloperLoginRequest> Request, FOnDeveloperLoginResponse OnComplete)
{
	const TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = ApiUrl;
	ApiRequest->Method = POST;
	ApiRequest->Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest->Payload = ConvertToJsonString(*Request.Get());
	SendRequest<FDeveloperLoginResponse>(ApiRequest, [OnComplete](TSharedPtr<FDeveloperLoginResponse> Response, bool bWasSuccessful, int32 StatusCode)
	{
		OnComplete.ExecuteIfBound(Response, bWasSuccessful && Response.IsValid());
	});

}
