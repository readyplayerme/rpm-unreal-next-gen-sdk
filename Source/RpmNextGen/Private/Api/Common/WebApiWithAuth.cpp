#include "Api/Common/WebApiWithAuth.h"
#include "RpmNextGen.h"
#include "Interfaces/IHttpResponse.h"

FWebApiWithAuth::FWebApiWithAuth() : AuthenticationStrategy(nullptr)
{
    FWebApi();
}

FWebApiWithAuth::FWebApiWithAuth(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy) : AuthenticationStrategy(InAuthenticationStrategy)
{
    FWebApi();
}

//TODO check if this is needed
FWebApiWithAuth::~FWebApiWithAuth()
{
    CancelAllRequests();
    AuthenticationStrategy = nullptr;
}

void FWebApiWithAuth::SetAuthenticationStrategy(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy)
{
    AuthenticationStrategy = InAuthenticationStrategy;
}
