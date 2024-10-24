#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"
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

// DECLARE_DELEGATE_ThreeParams(FOnRefreshTokenResponse, TSharedPtr<FApiRequest>, const FRefreshTokenResponse&, bool);
// DECLARE_DELEGATE_TwoParams(FOnSendLoginCodeResponse, TSharedPtr<FApiRequest>, bool);
// DECLARE_DELEGATE_ThreeParams(FOnLoginWithCodeResponse, TSharedPtr<FApiRequest>, const FLoginWithCodeResponse&, bool);
// DECLARE_DELEGATE_ThreeParams(FOnCreateUserResponse, TSharedPtr<FApiRequest>, const FCreateUserResponse&, bool);

class RPMNEXTGEN_API FAuthApi : public FWebApiWithAuth
{
	// enum class EAuthRequestType
	// {
	// 	RefreshToken,
	// 	SendLoginCode,
	// 	LoginWithCode,
	// 	CreateUser
	// };

public:
	// FOnRefreshTokenResponse OnRefreshTokenResponse;
	// FOnSendLoginCodeResponse OnSendLoginCodeResponse;
	// FOnLoginWithCodeResponse OnLoginWithCodeResponse;
	// FOnCreateUserResponse OnCreateUserResponse;
	
	FAuthApi();
	void RefreshToken(const FRefreshTokenRequest& Request, TFunction<void(TSharedPtr<FRefreshTokenResponse>, bool)> OnComplete);
	void SendLoginCode(const FSendLoginCodeRequest& Request, TFunction<void(TSharedPtr<FApiResponse>, bool)> OnComplete);
	void LoginWithCode(const FLoginWithCodeRequest& Request, TFunction<void(TSharedPtr<FLoginWithCodeResponse>, bool)> OnComplete);
	void CreateUser(const FCreateUserRequest& Request, TFunction<void(TSharedPtr<FCreateUserResponse>, bool)> OnComplete);

private:
	const URpmDeveloperSettings* RpmSettings;
};
