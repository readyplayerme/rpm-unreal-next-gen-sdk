#include "AssetSaver.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/Models/Asset.h"
#include "Interfaces/IHttpResponse.h"

const FString FAssetSaver::CacheFolderPath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/LocalCache");


FAssetSaver::FAssetSaver(): bIsImageLoaded(false), bIsGlbLoaded(false)
{
	Http = &FHttpModule::Get();
}

FAssetSaver::~FAssetSaver()
{
}

void FAssetSaver::SaveAssetToCache(const FString& BaseModelId, const FAsset* Asset)
{
	const FString Path = CacheFolderPath / BaseModelId;
	LoadAndSaveGlb(Asset->IconUrl, FString::Printf(TEXT("%s/%s.glb"), *Path, *Asset->Id));
	LoadAndSaveImage(Asset->IconUrl, FString::Printf(TEXT("%s/%s.png"), *Path, *Asset->Id));
}

void FAssetSaver::LoadAndSaveImage(const FString& Url, const FString& FilePath)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));

	// Capture TSharedPtr<FAssetSaver> in the lambda to ensure it is not destroyed prematurely
	TSharedPtr<FAssetSaver> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, FilePath](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->OnAssetLoaded(Request, Response, bWasSuccessful, FilePath);
	});

	Request->ProcessRequest();
}

void FAssetSaver::LoadAndSaveGlb(const FString& Url, const FString& FilePath)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));

	// Capture TSharedPtr<FAssetSaver> in the lambda to ensure it is not destroyed prematurely
	TSharedPtr<FAssetSaver> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, FilePath](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->OnAssetLoaded(Request, Response, bWasSuccessful, FilePath);
	});

	Request->ProcessRequest();
}

void FAssetSaver::OnAssetLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FString& FilePath)
{
	if(bWasSuccessful && Response.IsValid())
	{
		SaveToFile(FilePath, Response->GetContent());
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load image from url: %s"), *Request->GetURL());
	}
	if(FilePath.EndsWith(TEXT(".glb")))
	{
		bIsGlbLoaded = true;
	}
	else
	{
		bIsImageLoaded = true;
	}
	if(bIsImageLoaded && bIsGlbLoaded)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset saved to cache"));
		OnAssetSaved.ExecuteIfBound(true);
	}
}

void FAssetSaver::SaveToFile(const FString& FilePath, const TArray<uint8>& Data)
{
	if (FFileHelper::SaveArrayToFile(Data, *FilePath))
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully saved asset in local cache to: %s"), *FilePath);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to saved asset local cache to: %s"), *FilePath);
}
