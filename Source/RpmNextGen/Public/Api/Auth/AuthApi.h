#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApiWithAuth.h"

struct FApiResponse;
struct FCreateUserResponse;
struct FLoginWithCodeResponse;
class URpmDeveloperSettings;
struct FCreateUserRequest;
struct FLoginWithCodeRequest;
struct FSendLoginCodeRequest;
struct FRefreshTokenResponse;
struct FRefreshTokenRequest;

DECLARE_DELEGATE_TwoParams(FOnRefreshTokenResponse, TSharedPtr<FRefreshTokenResponse>, bool);
DECLARE_DELEGATE_OneParam(FOnSendLoginCodeResponse, bool);
DECLARE_DELEGATE_TwoParams(FOnLoginWithCodeResponse, TSharedPtr<const FLoginWithCodeResponse>, bool);
DECLARE_DELEGATE_TwoParams(FOnCreateUserResponse, TSharedPtr<FCreateUserResponse>, bool);

class RPMNEXTGEN_API FAuthApi : public FWebApi
{
public:
	FAuthApi();
	void RefreshToken(const FRefreshTokenRequest& Request, FOnRefreshTokenResponse OnComplete);
	void SendLoginCode(const FSendLoginCodeRequest& Request, FOnSendLoginCodeResponse OnComplete);
	void LoginWithCode(const FLoginWithCodeRequest& Request, FOnLoginWithCodeResponse OnComplete);
	void CreateUser(const FCreateUserRequest& Request, FOnCreateUserResponse OnComplete);

private:
	const URpmDeveloperSettings* RpmSettings;
};
