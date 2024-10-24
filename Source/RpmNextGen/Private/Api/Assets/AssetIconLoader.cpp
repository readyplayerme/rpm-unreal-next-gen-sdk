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
	CancelAllRequests();
}

void FAssetIconLoader::LoadIcon(const FAsset& Asset, bool bStoreInCache)
{
	FCachedAssetData StoredAsset;
	if (FAssetCacheManager::Get().GetCachedAsset(Asset.Id, StoredAsset))
	{
		TArray<uint8> IconData;
		if(FFileHelper::LoadFileToArray(IconData, * FFileUtility::GetFullPersistentPath(StoredAsset.RelativeIconFilePath)))
		{
			OnIconLoaded.ExecuteIfBound(Asset, IconData);
			return;
		}
	}
	TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
	ApiRequest->Url = Asset.IconUrl;
	ApiRequest->Method = GET;
	TWeakPtr<FAssetIconLoader> ThisPtr = SharedThis(this);
	SendRequest<TArray<uint8>>(ApiRequest, [ThisPtr, Asset, bStoreInCache](TSharedPtr<TArray<uint8>> Response, bool bWasSuccessful)
	{
		if(ThisPtr.IsValid())
		{
			if (bWasSuccessful && Response.IsValid())
			{
				FAssetLoadingContext Context(Asset, "", bStoreInCache);
				Context.Data = *Response.Get();
				if (bStoreInCache)
				{
					FAssetCacheManager::Get().StoreAndTrackIcon(Context);
				}
				ThisPtr.Pin()->OnIconLoaded.ExecuteIfBound(Asset, Context.Data);
				return;
			}
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load icon from URL: %s"), *Asset.IconUrl);
			ThisPtr.Pin()->OnIconLoaded.ExecuteIfBound(Asset, TArray<uint8>());
		}

	});

	// const TSharedRef<FAssetLoadingContext> Context = MakeShared<FAssetLoadingContext>(Asset, "", bStoreInCache);
	// TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	// Request->SetURL(Asset.IconUrl);
	// Request->SetVerb(TEXT("GET"));
	//
	// TWeakPtr<FAssetIconLoader> ThisPtr = SharedThis(this);
	// Request->OnProcessRequestComplete().BindLambda([ThisPtr, bStoreInCache, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	// {
	// 	if (ThisPtr.IsValid() && bWasSuccessful && Response.IsValid())
	// 	{
	// 		const auto Data = Response->GetContent();
	// 		if (bStoreInCache)
	// 		{
	// 			FAssetCacheManager::Get().StoreAndTrackIcon(*Context);
	// 		}
	// 		ThisPtr.Pin()->OnIconLoaded.ExecuteIfBound(Context->Asset, Context->Data);
	// 		return;
	// 	}
	// 	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load icon from URL: %s"), *Context->Asset.IconUrl);
	// 	ThisPtr.Pin()->OnIconLoaded.ExecuteIfBound(Context->Asset, TArray<uint8>());
	// });
	//
	// Request->ProcessRequest();
}

// void FAssetIconLoader::LoadIcon(TSharedRef<FAssetLoadingContext> Context)
// {
// 	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
// 	Request->SetURL(Context->Asset.IconUrl);
// 	Request->SetVerb(TEXT("GET"));
//
// 	TSharedPtr<FAssetIconLoader> ThisPtr = SharedThis(this);
// 	Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
// 	{
// 		ThisPtr->IconLoaded(Response, bWasSuccessful, Context);
// 	});
// 	Request->ProcessRequest();
// }
//
// void FAssetIconLoader::RequestIcon(const FAsset& Asset, TFunction<void(const TArray<uint8>&, bool)> OnResponse)
// {
// 	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
// 	Request->SetURL(Asset.IconUrl);
// 	Request->SetVerb(TEXT("GET"));
// 	TWeakPtr<FAssetIconLoader> ThisPtr = SharedThis(this);
// 	Request->OnProcessRequestComplete().BindLambda([ThisPtr, OnResponse](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
// 	{
// 		if (ThisPtr.IsValid() && bWasSuccessful && Response.IsValid())
// 		{
// 			OnResponse(Response->GetContent(), true);
// 			return;
// 		}
// 		OnResponse(TArray<uint8>(), false);
// 	});
// }
//
// void FAssetIconLoader::IconLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful,	const TSharedRef<FAssetLoadingContext>& Context)
// {
// 	if (bWasSuccessful && Response.IsValid())
// 	{
// 		Context->Data = Response->GetContent();
// 		if (Context->bStoreInCache)
// 		{
// 			FAssetCacheManager::Get().StoreAndTrackIcon(*Context);
// 		}
// 		OnIconLoaded.ExecuteIfBound(Context->Asset, Context->Data);
// 		return;
// 	}
// 	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load icon from URL: %s"), *Context->Asset.IconUrl);
// 	OnIconLoaded.ExecuteIfBound(Context->Asset, TArray<uint8>());
// }
