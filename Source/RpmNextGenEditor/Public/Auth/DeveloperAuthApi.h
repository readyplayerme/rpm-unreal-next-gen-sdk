#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"

struct FDeveloperLoginResponse;
struct FDeveloperLoginRequest;

DECLARE_DELEGATE_TwoParams(FOnDeveloperLoginResponse, TSharedPtr<FDeveloperLoginResponse>, bool);

class RPMNEXTGENEDITOR_API FDeveloperAuthApi : public FWebApi
{
public:	
	FDeveloperAuthApi();

	void LoginWithEmail(TSharedPtr<FDeveloperLoginRequest> Request, FOnDeveloperLoginResponse OnComplete);

private:
	FString ApiUrl;
};
