#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"

struct FDeveloperLoginResponse;
struct FDeveloperLoginRequest;

DECLARE_DELEGATE_TwoParams(FOnDeveloperLoginResponse, const FDeveloperLoginResponse&, bool);

class RPMNEXTGENEDITOR_API FDeveloperAuthApi : public FWebApi
{
public:
	FOnDeveloperLoginResponse OnLoginResponse;
	
	FDeveloperAuthApi();

	void HandleLoginResponse(FString JsonData, bool bIsSuccessful) const;
	void LoginWithEmail(FDeveloperLoginRequest Request);

private:
	FString ApiUrl;
};
