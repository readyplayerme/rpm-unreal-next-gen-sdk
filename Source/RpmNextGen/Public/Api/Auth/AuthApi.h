#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"

struct FCreateUserResponse;
struct FLoginWithCodeResponse;
struct FSendLoginCodeResponse;
class URpmDeveloperSettings;
struct FCreateUserRequest;
struct FLoginWithCodeRequest;
struct FSendLoginCodeRequest;
struct FRefreshTokenResponse;
struct FRefreshTokenRequest;

DECLARE_DELEGATE_ThreeParams(FOnRefreshTokenResponse, TSharedPtr<FApiRequest>, const FRefreshTokenResponse&, bool);
DECLARE_DELEGATE_ThreeParams(FOnSendLoginCodeResponse, TSharedPtr<FApiRequest>, const FSendLoginCodeResponse&, bool);
DECLARE_DELEGATE_ThreeParams(FOnLoginWithCodeResponse, TSharedPtr<FApiRequest>, const FLoginWithCodeResponse&, bool);
DECLARE_DELEGATE_ThreeParams(FOnCreateUserResponse, TSharedPtr<FApiRequest>, const FCreateUserResponse&, bool);

class RPMNEXTGEN_API FAuthApi : public FWebApi
{
public:
	FOnRefreshTokenResponse OnRefreshTokenResponse;
	FOnSendLoginCodeResponse OnSendLoginCodeResponse;
	FOnLoginWithCodeResponse OnLoginWithCodeResponse;
	FOnCreateUserResponse OnCreateUserResponse;
	
	FAuthApi();
	void RefreshToken(const FRefreshTokenRequest& Request);
	void SendLoginCode(const FSendLoginCodeRequest& Request);
	void LoginWithCode(const FLoginWithCodeRequest& Request);
	void CreateUser(const FCreateUserRequest& Request);
	
	void OnProcessComplete(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	const URpmDeveloperSettings* RpmSettings;
};
