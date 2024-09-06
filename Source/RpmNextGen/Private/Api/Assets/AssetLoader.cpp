#include "Api/Assets/AssetLoader.h"

#include "HttpModule.h"
#include "Api/Assets/Models/Asset.h"
#include "Cache/AssetStorageManager.h"
#include "Interfaces/IHttpResponse.h"
#include "RpmNextGenEditor/Public/EditorAssetLoader.h"

FAssetLoader::FAssetLoader()
{
	Http = &FHttpModule::Get();
}

FAssetLoader::~FAssetLoader()
{	
}

void FAssetLoader::LoadAssetGlb(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache)
{
	FAssetSaveData StoredAsset;
	if(FAssetStorageManager::Get().GetCachedAsset(Asset.Id, StoredAsset))
	{
		TArray<uint8> GlbData;
		FFileHelper::LoadFileToArray(GlbData, *StoredAsset.GlbFilePath);
		OnAssetGlbLoaded.ExecuteIfBound(Asset, GlbData);
		return;
	}
	const TSharedRef<FAssetLoadingContext> Context = MakeShared<FAssetLoadingContext>(Asset, BaseModelId, bStoreInCache);
	Context->bIsGLb = true;
	LoadAssetGlb(Context);
}

void FAssetLoader::LoadAssetIcon(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache)
{
	FAssetSaveData StoredAsset;
	if(FAssetStorageManager::Get().GetCachedAsset(Asset.Id, StoredAsset))
	{
		TArray<uint8> IconData;
		FFileHelper::LoadFileToArray(IconData, *StoredAsset.IconFilePath);
		OnAssetGlbLoaded.ExecuteIfBound(Asset, IconData);
		return;
	}
	const TSharedRef<FAssetLoadingContext> Context = MakeShared<FAssetLoadingContext>(Asset, BaseModelId, bStoreInCache);
	Context->bIsGLb = false;
	LoadAssetIcon(Context);
}

void FAssetLoader::LoadAssetIcon(TSharedRef<FAssetLoadingContext> Context)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Context->Asset.IconUrl);
	Request->SetVerb(TEXT("GET"));

	TSharedPtr<FAssetLoader> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->AssetIconLoaded(Response, bWasSuccessful, Context);
	});
	Request->ProcessRequest();
}

void FAssetLoader::AssetIconLoaded(TSharedPtr<IHttpResponse> Response, const bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context)
{
	if (bWasSuccessful && Response.IsValid())
	{
		Context->Data = Response->GetContent();
		if(!Context->bStoreInCache)
		{
			FAssetStorageManager::Get().StoreAndTrackAsset(*Context);
		}
		OnAssetIconLoaded.ExecuteIfBound(Context->Asset, Context->Data);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load image from URL: %s"), *Context->Asset.IconUrl);
	OnAssetIconLoaded.ExecuteIfBound(Context->Asset, TArray<uint8>());
}

void FAssetLoader::LoadAssetGlb(TSharedRef<FAssetLoadingContext> Context)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Context->Asset.GlbUrl);
	Request->SetVerb(TEXT("GET"));

	TSharedPtr<FAssetLoader> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->AssetGlbLoaded(Response, bWasSuccessful, Context);
	});
	Request->ProcessRequest();
}

void FAssetLoader::AssetGlbLoaded(TSharedPtr<IHttpResponse> Response, const bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context)
{
	if (bWasSuccessful && Response.IsValid())
	{
		Context->Data = Response->GetContent();

		if (Context->bStoreInCache)
		{
			FAssetStorageManager::Get().StoreAndTrackAsset(*Context);
		}
		OnAssetGlbLoaded.ExecuteIfBound(Context->Asset, Context->Data);

		UE_LOG(LogTemp, Log, TEXT("Asset loaded successfully."));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load .glb model from URL: %s"), *Context->Asset.GlbUrl);
	OnAssetGlbLoaded.ExecuteIfBound(Context->Asset, TArray<uint8>());
}
