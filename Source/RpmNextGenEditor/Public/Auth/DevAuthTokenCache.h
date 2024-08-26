#pragma once

#include "CoreMinimal.h"

struct FDeveloperAuth;

class RPMNEXTGENEDITOR_API FDevAuthTokenCache
{
public:
	static FDeveloperAuth GetAuthData();
	static void SetAuthData(const FDeveloperAuth& DevAuthData);
	static void ClearAuthData();
private:
	static constexpr const TCHAR* CacheKeyName = TEXT("Name");
	static constexpr const TCHAR* CacheKeyToken = TEXT("Token");
	static constexpr const TCHAR* CacheKeyRefreshToken = TEXT("RefreshToken");
	static constexpr const TCHAR* CacheKeyIsDemo = TEXT("IsDemo");
	
	static FDeveloperAuth AuthData;
	static bool bIsInitialized;
	static void Initialize();
};
