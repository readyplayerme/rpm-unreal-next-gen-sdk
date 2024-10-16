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

FAssetApi::~FAssetApi()
{
}

void FAssetApi::Initialize()
{
	if (bIsInitialized)	return;
	
	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
	OnRequestComplete.BindRaw(this, &FAssetApi::HandleAssetResponse);
	if (Settings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Application ID is empty. Please set the Application ID in the Ready Player Me Developer Settings"));
	}

	if (!Settings->ApiKey.IsEmpty() || Settings->ApiProxyUrl.IsEmpty())
	{
		SetAuthenticationStrategy(MakeShared<FApiKeyAuthStrategy>());
	}
	bIsInitialized = true;
}

void FAssetApi::ListAssetsAsync(const FAssetListRequest& Request)
{
	if(ApiRequestStrategy == EApiRequestStrategy::CacheOnly)
	{
		LoadAssetsFromCache(Request.BuildQueryMap());
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
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets"), *ApiBaseUrl);
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = Url;
	ApiRequest->Method = GET;
	ApiRequest->QueryParams = Request.BuildQueryMap();
	
	DispatchRawWithAuth(ApiRequest);
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
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = Url;
	ApiRequest->Method = GET;
	
	DispatchRawWithAuth( ApiRequest);
}

void FAssetApi::HandleAssetResponse(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const bool bIsTypeRequest = ApiRequest->Url.Contains(TEXT("/types"));
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		FAssetTypeListResponse AssetTypeListResponse;
		
		if (bIsTypeRequest && FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &AssetTypeListResponse, 0, 0))
		{
			OnListAssetTypeResponse.ExecuteIfBound(AssetTypeListResponse, true);
			return;
		}
		FAssetListResponse AssetListResponse;
		if (!bIsTypeRequest && FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &AssetListResponse, 0, 0))
		{
			OnListAssetsResponse.ExecuteIfBound(AssetListResponse, true);
			return;
		}
	}
	
	if(ApiRequestStrategy == EApiRequestStrategy::ApiOnly)
	{
		if(bIsTypeRequest)
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("AssetApi:ListAssetTypesAsync request failed: %s"), *Response->GetContentAsString());
			OnListAssetTypeResponse.ExecuteIfBound(FAssetTypeListResponse(), false);
			return;
		}
		UE_LOG(LogReadyPlayerMe, Error, TEXT("AssetApi:ListAssetsAsync request failed: %s"), *Response->GetContentAsString());
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	if(bIsTypeRequest)
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("FAssetApi::ListAssetTypesAsync request failed, falling back to cache."));
		LoadAssetTypesFromCache();
		return;
	}
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("FAssetApi::ListAssetsAsync request failed, falling back to cache."));
	LoadAssetsFromCache(ApiRequest->QueryParams);
}

void FAssetApi::LoadAssetsFromCache(TMap<FString, FString> QueryParams)
{
	if(QueryParams.Num() < 1)
	{
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	const FString TypeKey = TEXT("type");
	const FString ExcludeTypeKey = TEXT("excludeType");
	FString Type = QueryParams.Contains(TypeKey) ? *QueryParams.Find(TypeKey) : FString();
	FString ExcludeType = QueryParams.Contains(ExcludeTypeKey) ? *QueryParams.Find(ExcludeTypeKey) : FString();
	TArray<FCachedAssetData> CachedAssets;
	
	if(ExcludeType.IsEmpty())
	{
		CachedAssets = FAssetCacheManager::Get().GetAssetsOfType(Type);
	}
	else
	{
		auto ExtractQueryString = TEXT("excludeType=") + ExcludeType;
		auto ExtractQueryArray = ExtractQueryValues(ExcludeType, ExcludeTypeKey);
		CachedAssets = FAssetCacheManager::Get().GetAssetsExcludingTypes(ExtractQueryArray);
	}

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

TArray<FString> FAssetApi::ExtractQueryValues(const FString& QueryString, const FString& Key)
{
	TArray<FString> Values;
	TArray<FString> Pairs;
	
	// Split the query string by '&' to separate each key-value pair
	QueryString.ParseIntoArray(Pairs, TEXT("&"), true);

	// Iterate through all pairs and check if they match the key
	for (const FString& Pair : Pairs)
	{
		FString KeyPart, ValuePart;

		// Split the pair by '=' to get the key and value
		if (Pair.Split(TEXT("="), &KeyPart, &ValuePart))
		{
			// If the key matches the one we're looking for, add the value to the array
			if (KeyPart.Equals(Key, ESearchCase::IgnoreCase))
			{
				Values.Add(ValuePart);
			}
		}
	}

	return Values;
}
