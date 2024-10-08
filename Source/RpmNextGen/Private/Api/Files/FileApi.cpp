#include "Api/Files/FileApi.h"

#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/Models/Asset.h"
#include "Interfaces/IHttpResponse.h"

FFileApi::FFileApi()
{
}

FFileApi::~FFileApi()
{
}

void FFileApi::LoadFileFromUrl(const FString& URL)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileApi::FileRequestComplete);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

void FFileApi::LoadAssetFileFromUrl(const FString& URL, const FAsset& Asset)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileApi::AssetFileRequestComplete, Asset);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

void FFileApi::FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString URL = Request->GetURL();
	const FString FileName = FPaths::GetCleanFilename(URL);

	if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
	{
		const TArray<uint8>& Content = Response->GetContent(); 
		OnFileRequestComplete.ExecuteIfBound(Content, FileName);
		return;
	}

	const int32 ResponseCode = Response.IsValid() ? Response->GetResponseCode() : -1;
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load file from URL. Response code: %d, URL: %s"), ResponseCode, *URL);
	OnFileRequestComplete.ExecuteIfBound(TArray<uint8>(), FileName);
}

void FFileApi::AssetFileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FAsset Asset)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
	{
		const TArray<uint8>& Content = Response->GetContent(); 
		OnAssetFileRequestComplete.ExecuteIfBound(Content, Asset);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("Failed to load file from URL. Try loading from cache"));
	OnAssetFileRequestComplete.ExecuteIfBound(TArray<uint8>(), Asset);
}

bool FFileApi::LoadFileFromPath(const FString& Path, TArray<uint8>& OutContent)
{
	if (!FPaths::FileExists(Path))
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Path does not exist %s"), *Path);
		return false;
	}

	return FFileHelper::LoadFileToArray(OutContent, *Path);
}
