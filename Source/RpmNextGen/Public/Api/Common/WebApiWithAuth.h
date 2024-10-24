#pragma once

#include "CoreMinimal.h"
#include "WebApi.h"
#include "Api/Auth/IAuthenticationStrategy.h"

class IAuthenticationStrategy;

class RPMNEXTGEN_API FWebApiWithAuth : public FWebApi
{
public:
	FWebApiWithAuth();
	FWebApiWithAuth(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy);
	virtual ~FWebApiWithAuth() override;
	void SetAuthenticationStrategy(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy);
	
	// void OnAuthComplete(TSharedPtr<FApiRequest> ApiRequest, bool bWasSuccessful); 
	// void OnAuthTokenRefreshed(TSharedPtr<FApiRequest> ApiRequest, const FRefreshTokenResponseBody& Response, bool bWasSuccessful); 

	template <typename T>
	void SendRequestWithAuth(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<T>, bool)> OnResponse);
	
protected:
	TSharedPtr<IAuthenticationStrategy> AuthenticationStrategy;

};

template <typename T>
void FWebApiWithAuth::SendRequestWithAuth(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<T>, bool)> OnResponse)
{
	if (AuthenticationStrategy)
	{
		AuthenticationStrategy->AddAuthToRequest(ApiRequest, [this, ApiRequest, OnResponse](TSharedPtr<FApiRequest> AuthenticatedRequest, bool bAuthSuccess)
		{
			if (bAuthSuccess)
			{
				SendRequest<T>(AuthenticatedRequest, OnResponse);
			}
			else
			{
				OnResponse(nullptr, false);
			}
		});
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AuthStrategy is null, cannot send authenticated request."));
		OnResponse(nullptr, false);
	}
}
