#include "Api/Assets/AssetApi.h"

#include "RpmNextGen.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Cache/AssetCacheManager.h"
#include "Interfaces/IHttpResponse.h"

struct FCachedAssetData;
struct FAssetTypeListRequest;
const FString FAssetApi::BaseModelType = TEXT("baseModel");

FAssetApi::FAssetApi() : ApiRequestStrategy(EApiRequestStrategy::FallbackToCache)
{
	Initialize();
}

FAssetApi::FAssetApi(EApiRequestStrategy InApiRequestStrategy) : ApiRequestStrategy(InApiRequestStrategy)
{
	Initialize();
}

void FAssetApi::Initialize()
{
	AssetListApi = MakeShared<FWebApiWithAuth>();
	AssetTypeListApi = MakeShared<FWebApiWithAuth>();
	AssetListApi->OnRequestComplete.BindRaw(this, &FAssetApi::HandleAssetListResponse);
	AssetTypeListApi->OnRequestComplete.BindRaw(this, &FAssetApi::HandleAssetTypeListResponse);

	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();

	if (Settings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Application ID is empty. Please set the Application ID in the Ready Player Me Developer Settings"));
	}

	if (!Settings->ApiKey.IsEmpty() || Settings->ApiProxyUrl.IsEmpty())
	{
		AssetListApi->SetAuthenticationStrategy(new FApiKeyAuthStrategy());
		AssetTypeListApi->SetAuthenticationStrategy(new FApiKeyAuthStrategy());
	}
}

void FAssetApi::SetAuthenticationStrategy(IAuthenticationStrategy* InAuthStrategy)
{
	AssetTypeListApi->SetAuthenticationStrategy(InAuthStrategy);
	AssetListApi->SetAuthenticationStrategy(InAuthStrategy);
}

void FAssetApi::ListAssetsAsync(const FAssetListRequest& Request)
{
	if(ApiRequestStrategy == EApiRequestStrategy::CacheOnly)
	{
		LoadAssetsFromCache();
		return;
	}
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	ApiBaseUrl = RpmSettings->GetApiBaseUrl();
	if(RpmSettings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Application ID is empty"));
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	FString QueryString = Request.BuildQueryString();
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets%s"), *ApiBaseUrl, *QueryString);
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = Url;
	ApiRequest.Method = GET;
	AssetListApi->DispatchRawWithAuth(ApiRequest);
}

void FAssetApi::ListAssetTypesAsync(const FAssetTypeListRequest& Request)
{
	if(ApiRequestStrategy == EApiRequestStrategy::CacheOnly)
	{
		LoadAssetTypesFromCache();
		return;
	}
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	ApiBaseUrl = Settings->GetApiBaseUrl();
	if(Settings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Application ID is empty"));
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	FString QueryString = Request.BuildQueryString();
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets/types%s"), *ApiBaseUrl, *QueryString);
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = Url;
	ApiRequest.Method = GET;

	AssetTypeListApi->DispatchRawWithAuth( ApiRequest);
}

void FAssetApi::HandleAssetListResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {    	
    	FAssetListResponse AssetListResponse;
    	if (FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &AssetListResponse, 0, 0))
    	{
    		OnListAssetsResponse.ExecuteIfBound(AssetListResponse, true);
    		return;
    	}

        UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to parse JSON into known structs from response: %s"), *Response->GetContentAsString());
    }
	
	if(ApiRequestStrategy == EApiRequestStrategy::ApiOnly)
	{
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	
    UE_LOG(LogReadyPlayerMe, Error, TEXT("API Response was unsuccessful, loading from cache."));


	LoadAssetsFromCache();
}

void FAssetApi::HandleAssetTypeListResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
    {    	
	    	FAssetTypeListResponse AssetTypeListResponse;
	    	if (FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &AssetTypeListResponse, 0, 0))
	    	{
	    		OnListAssetTypeResponse.ExecuteIfBound(AssetTypeListResponse, true);
	    		return;
	    	}

        UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to parse JSON into known structs from response: %s"), *Response->GetContentAsString());
    }

	if(ApiRequestStrategy == EApiRequestStrategy::ApiOnly)
	{
		OnListAssetTypeResponse.ExecuteIfBound(FAssetTypeListResponse(), false);
		return;
	}
	
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("API Response was unsuccessful for asset types, falling back to cache.")); 
	LoadAssetTypesFromCache();
}

void FAssetApi::LoadAssetsFromCache()
{
	TArray<FCachedAssetData> CachedAssets = FAssetCacheManager::Get().GetAssetsOfType(BaseModelType);
	if (CachedAssets.Num() > 0)
	{
		FAssetListResponse AssetListResponse;
		for (const FCachedAssetData& CachedAsset : CachedAssets)
		{
			FAsset Asset = CachedAsset.ToAsset();
			AssetListResponse.Data.Add(Asset);
		}
		OnListAssetsResponse.ExecuteIfBound(AssetListResponse, true);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("No assets found in the cache."));
	OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
}

void FAssetApi::LoadAssetTypesFromCache()
{
	TArray<FString> AssetTypes = FAssetCacheManager::Get().LoadAssetTypes();
	if (AssetTypes.Num() > 0)
	{
		FAssetTypeListResponse AssetListResponse;
		AssetListResponse.Data.Append(AssetTypes);
		OnListAssetTypeResponse.ExecuteIfBound(AssetListResponse, true);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("No assets found in the cache."));
	OnListAssetTypeResponse.ExecuteIfBound(FAssetTypeListResponse(), false);
}
