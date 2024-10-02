#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"

struct FRefreshTokenResponse;
struct FRefreshTokenRequest;

DECLARE_DELEGATE_TwoParams(FOnRefreshTokenResponse, const FRefreshTokenResponse&, bool);

class RPMNEXTGEN_API FAuthApi :  public FWebApi
{
public:
	FOnRefreshTokenResponse OnRefreshTokenResponse;
	
	FAuthApi();
	void RefreshToken(const FRefreshTokenRequest& Request);
	
	virtual void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FApiRequest& ApiRequest) override;

private:
	FString ApiUrl;
};
