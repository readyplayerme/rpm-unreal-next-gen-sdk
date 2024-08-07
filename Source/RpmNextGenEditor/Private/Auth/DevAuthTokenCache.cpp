#include "Auth/DevAuthTokenCache.h"
#include "EditorCache.h"
#include "Misc/ConfigCacheIni.h"

FDeveloperAuth DevAuthTokenCache::AuthData = FDeveloperAuth();
bool DevAuthTokenCache::bIsInitialized = false;

void DevAuthTokenCache::Initialize()
{
	if (!bIsInitialized)
	{
		AuthData.Name = EditorCache::GetString(CacheKeyName, FString());
		AuthData.Token = EditorCache::GetString(CacheKeyToken, FString());
		AuthData.RefreshToken = EditorCache::GetString(CacheKeyRefreshToken, FString());
		AuthData.IsDemo = EditorCache::GetBool(CacheKeyIsDemo, false);

		if (!AuthData.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("DevAuthTokenCache: Invalid AuthData %s"), *AuthData.ToJsonString());
			ClearAuthData();
		}

		bIsInitialized = true;
	}
}

FDeveloperAuth DevAuthTokenCache::GetAuthData()
{
	Initialize();	
	return AuthData;
}

void DevAuthTokenCache::SetAuthData(const FDeveloperLoginResponse& LoginResponse)
{
	AuthData = FDeveloperAuth(LoginResponse.Data, false);
	EditorCache::SetString( CacheKeyName, AuthData.Name);
	EditorCache::SetString( CacheKeyToken, AuthData.Token);
	EditorCache::SetString( CacheKeyRefreshToken, AuthData.RefreshToken);
	EditorCache::SetBool( CacheKeyIsDemo, AuthData.IsDemo);
}

void DevAuthTokenCache::ClearAuthData()
{
	AuthData = FDeveloperAuth();
	EditorCache::RemoveKey(CacheKeyName);
	EditorCache::RemoveKey(CacheKeyToken);
	EditorCache::RemoveKey(CacheKeyRefreshToken);
	EditorCache::RemoveKey(CacheKeyIsDemo);
}
