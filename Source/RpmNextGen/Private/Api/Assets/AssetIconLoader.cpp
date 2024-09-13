#include "Api/Assets/AssetIconLoader.h"
#include "Api/Assets/AssetLoaderContext.h"
#include "HttpModule.h"
#include "Cache/AssetCacheManager.h"
#include "Interfaces/IHttpResponse.h"

FAssetIconLoader::FAssetIconLoader()
{
	Http = &FHttpModule::Get();
}

FAssetIconLoader::~FAssetIconLoader()
{
}

void FAssetIconLoader::LoadIcon(const FAsset& Asset, bool bStoreInCache)
{
	FCachedAssetData StoredAsset;
	if (FAssetCacheManager::Get().GetCachedAsset(Asset.Id, StoredAsset))
	{
		TArray<uint8> IconData;
		if(FFileHelper::LoadFileToArray(IconData, *StoredAsset.IconFilePath))
		{
			OnIconLoaded.ExecuteIfBound(Asset, IconData);
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Loading icon from cache"));
			return;
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Unable to load icon from cache"));
	}
	const TSharedRef<FAssetLoadingContext> Context = MakeShared<FAssetLoadingContext>(Asset, "", bStoreInCache);
	LoadIcon(Context);
}

void FAssetIconLoader::LoadIcon(TSharedRef<FAssetLoadingContext> Context)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Context->Asset.IconUrl);
	Request->SetVerb(TEXT("GET"));

	TSharedPtr<FAssetIconLoader> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->IconLoaded(Response, bWasSuccessful, Context);
	});
	Request->ProcessRequest();
}

void FAssetIconLoader::IconLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful,	const TSharedRef<FAssetLoadingContext>& Context)
{
	if (bWasSuccessful && Response.IsValid())
	{
		Context->Data = Response->GetContent();
		if (Context->bStoreInCache)
		{
			FAssetCacheManager::Get().StoreAndTrackIcon(*Context);
		}
		OnIconLoaded.ExecuteIfBound(Context->Asset, Context->Data);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load icon from URL: %s"), *Context->Asset.IconUrl);
	OnIconLoaded.ExecuteIfBound(Context->Asset, TArray<uint8>());
}
