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

void FAssetLoader::LoadAsset(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache)
{
	const TSharedRef<FAssetLoadingContext> Context = MakeShared<FAssetLoadingContext>(Asset, BaseModelId, bStoreInCache);
	LoadAssetImage(Context);
}

void FAssetLoader::LoadAssetImage(TSharedRef<FAssetLoadingContext> Context)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Context->Asset.IconUrl);
	Request->SetVerb(TEXT("GET"));

	TSharedPtr<FAssetLoader> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->AssetImageLoaded(Response, bWasSuccessful, Context);
	});
	Request->ProcessRequest();
}

void FAssetLoader::AssetImageLoaded(TSharedPtr<IHttpResponse> Response, const bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context)
{
	if (bWasSuccessful && Response.IsValid())
	{
		Context->ImageData = Response->GetContent();
		LoadAssetModel(Context);
		OnAssetImageLoaded.ExecuteIfBound(Context->ImageData);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load image from URL: %s"), *Context->Asset.IconUrl);
	LoadAssetModel(Context);
	OnAssetImageLoaded.ExecuteIfBound(TArray<uint8>());
}

void FAssetLoader::LoadAssetModel(TSharedRef<FAssetLoadingContext> Context)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Context->Asset.GlbUrl);
	Request->SetVerb(TEXT("GET"));

	TSharedPtr<FAssetLoader> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, Context](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->AssetModelLoaded(Response, bWasSuccessful, Context);
	});
	Request->ProcessRequest();
}

void FAssetLoader::AssetModelLoaded(TSharedPtr<IHttpResponse> Response, const bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context)
{
	if (bWasSuccessful && Response.IsValid())
	{
		Context->GlbData = Response->GetContent();

		if (Context->bStoreInCache)
		{
			FAssetStorageManager::Get().SaveAssetAndTrack(*Context);
			OnAssetSaved.ExecuteIfBound(FAssetSaveData(Context->Asset, Context->BaseModelId));
		}
		OnAssetGlbLoaded.ExecuteIfBound(Context->GlbData);

		UE_LOG(LogTemp, Log, TEXT("Asset loaded successfully."));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load .glb model from URL: %s"), *Context->Asset.GlbUrl);
	OnAssetGlbLoaded.ExecuteIfBound(TArray<uint8>());
	OnAssetSaved.ExecuteIfBound(FAssetSaveData());
}
