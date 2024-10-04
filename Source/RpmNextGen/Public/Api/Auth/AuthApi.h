#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"

struct FRefreshTokenResponse;
struct FRefreshTokenRequest;

DECLARE_DELEGATE_ThreeParams(FOnRefreshTokenResponse, TSharedPtr<FApiRequest>, const FRefreshTokenResponse&, bool);

class RPMNEXTGEN_API FAuthApi :  public FWebApi
{
public:
	FOnRefreshTokenResponse OnRefreshTokenResponse;
	
	FAuthApi();
	void RefreshToken(const FRefreshTokenRequest& Request);
	
	void OnProcessComplete(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	FString ApiUrl;
};
