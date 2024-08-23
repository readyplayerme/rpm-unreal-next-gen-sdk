#include "Auth/DevAuthTokenCache.h"
#include "EditorCache.h"
#include "Misc/ConfigCacheIni.h"

FDeveloperAuth DevAuthTokenCache::AuthData = FDeveloperAuth();
bool DevAuthTokenCache::bIsInitialized = false;

void DevAuthTokenCache::Initialize()
{
	if (!bIsInitialized)
	{
		AuthData.Name = FEditorCache::GetString(CacheKeyName, FString());
		AuthData.Token = FEditorCache::GetString(CacheKeyToken, FString());
		AuthData.RefreshToken = FEditorCache::GetString(CacheKeyRefreshToken, FString());
		AuthData.IsDemo = FEditorCache::GetBool(CacheKeyIsDemo, false);

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

void DevAuthTokenCache::SetAuthData(const FDeveloperAuth& DevAuthData)
{
	AuthData = DevAuthData;
	FEditorCache::SetString( CacheKeyName, AuthData.Name);
	FEditorCache::SetString( CacheKeyToken, AuthData.Token);
	FEditorCache::SetString( CacheKeyRefreshToken, AuthData.RefreshToken);
	FEditorCache::SetBool( CacheKeyIsDemo, AuthData.IsDemo);
}

void DevAuthTokenCache::ClearAuthData()
{
	AuthData = FDeveloperAuth();
	FEditorCache::RemoveKey(CacheKeyName);
	FEditorCache::RemoveKey(CacheKeyToken);
	FEditorCache::RemoveKey(CacheKeyRefreshToken);
	FEditorCache::RemoveKey(CacheKeyIsDemo);
}
