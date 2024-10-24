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
	template <typename T>
	void SendRequestWithAuth(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<T>, bool, int32)> OnResponse);
	
protected:
	TSharedPtr<IAuthenticationStrategy> AuthenticationStrategy;

};

template <typename T>
void FWebApiWithAuth::SendRequestWithAuth(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<T>, bool, int32)> OnResponse)
{
    if (AuthenticationStrategy)
    {
        AuthenticationStrategy->AddAuthToRequest(ApiRequest, [this, ApiRequest, OnResponse](TSharedPtr<FApiRequest> AuthenticatedRequest, bool bAddAuthSuccess)
        {
            if (bAddAuthSuccess)
            {
                SendRequest<T>(AuthenticatedRequest, [this, ApiRequest, OnResponse](TSharedPtr<T> Response, bool bWasSuccessful, int32 StatusCode)
                {
                    if (!bWasSuccessful && Response.IsValid() && StatusCode == 401)
                    {
                        AuthenticationStrategy->TryRefresh(ApiRequest, [this, ApiRequest, OnResponse, StatusCode](TSharedPtr<FApiRequest> RefreshedRequest, const FRefreshTokenResponse& RefreshResponse, bool bRefreshSuccess)
                        {
                            if (bRefreshSuccess)
                            {
                                SendRequest<T>(RefreshedRequest, OnResponse);
                            }
                            else
                            {
                                OnResponse(nullptr, false, StatusCode);
                            }
                        });
                    }
                    else
                    {
                        OnResponse(Response, bWasSuccessful, StatusCode);
                    }
                });
            }
            else
            {
                OnResponse(nullptr, false, -1);
            }
        });
    }
    else
    {
        SendRequest<T>(ApiRequest, [this, ApiRequest, OnResponse](TSharedPtr<T> Response, bool bWasSuccessful, int32 StatusCode)
        {
            if (!bWasSuccessful && Response.IsValid() && StatusCode == 401)
            {
                AuthenticationStrategy->TryRefresh(ApiRequest, [this, ApiRequest, OnResponse, StatusCode](TSharedPtr<FApiRequest> RefreshedRequest, const FRefreshTokenResponse& RefreshResponse, bool bRefreshSuccess)
                {
                    if (bRefreshSuccess)
                    {
                        SendRequest<T>(RefreshedRequest, OnResponse);
                    }
                    else
                    {
                        OnResponse(nullptr, false, StatusCode);
                    }
                });
            }
            else
            {
                OnResponse(Response, bWasSuccessful, StatusCode);
            }
        });
    }
}
