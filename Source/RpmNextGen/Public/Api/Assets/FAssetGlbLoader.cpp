#include "FAssetGlbLoader.h"

#include "AssetLoaderContext.h"
#include "HttpModule.h"
#include "Cache/AssetCacheManager.h"
#include "Interfaces/IHttpResponse.h"

FAssetGlbLoader::FAssetGlbLoader()
{
    Http = &FHttpModule::Get();
}

FAssetGlbLoader::~FAssetGlbLoader()
{
}

void FAssetGlbLoader::LoadGlb(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache)
{
    FCachedAssetData StoredAsset;
    if (FAssetCacheManager::Get().GetCachedAsset(Asset.Id, StoredAsset))
    {
        TArray<uint8> GlbData;
        FFileHelper::LoadFileToArray(GlbData, *StoredAsset.GlbPathsByBaseModelId[BaseModelId]);
        OnGlbLoaded.ExecuteIfBound(Asset, GlbData);
        UE_LOG(LogReadyPlayerMe, Log, TEXT("Loading GLB from cache"));
        return;
    }
    const TSharedRef<FAssetLoadingContext> Context = MakeShared<FAssetLoadingContext>(Asset, BaseModelId, bStoreInCache);
    LoadGlb(Context);
}

void FAssetGlbLoader::LoadGlb(TSharedRef<FAssetLoadingContext> Context)
{
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    Request->SetURL(Context->Asset.GlbUrl);
    Request->SetVerb(TEXT("GET"));

    TSharedPtr<FAssetGlbLoader> ThisPtr = SharedThis(this);
    Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        ThisPtr->GlbLoaded(Response, bWasSuccessful, Context);
    });
    Request->ProcessRequest();
}

void FAssetGlbLoader::GlbLoaded(TSharedPtr<IHttpResponse> Response, const bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context)
{
    if (bWasSuccessful && Response.IsValid())
    {
        Context->Data = Response->GetContent();
        if (Context->bStoreInCache)
        {
            FAssetCacheManager::Get().StoreAndTrackGlb(*Context);
        }
        OnGlbLoaded.ExecuteIfBound(Context->Asset, Context->Data);
        UE_LOG(LogTemp, Log, TEXT("GLB loaded successfully."));
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("Failed to load GLB from URL: %s"), *Context->Asset.GlbUrl);
    OnGlbLoaded.ExecuteIfBound(Context->Asset, TArray<uint8>());
}