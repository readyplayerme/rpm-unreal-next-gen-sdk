#pragma once

#include "CoreMinimal.h"
#include "WebApi.h"
#include "Api/Auth/IAuthenticationStrategy.h"


class RPMNEXTGEN_API FWebApiWithAuth : public FWebApi
{
public:
	FWebApiWithAuth();
	FWebApiWithAuth(IAuthenticationStrategy* InAuthenticationStrategy);

	void SetAuthenticationStrategy(IAuthenticationStrategy* InAuthenticationStrategy);

	// template <typename TRequestBody>
	// void DispatchWithAuth(
	// 	const TFApiRequest<TRequestBody>& Data
	// );
	
	void OnAuthComplete(bool bWasSuccessful); 
	void OnAuthTokenRefreshed(const FRefreshTokenResponseBody& Response, bool bWasSuccessful); 

	void DispatchRawWithAuth(FApiRequest& Data);
protected:
	FApiRequest* ApiRequestData;

	virtual void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;

private:
	IAuthenticationStrategy* AuthenticationStrategy;
};
