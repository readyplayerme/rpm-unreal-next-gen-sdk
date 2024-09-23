﻿#include "Api/Files/FileApi.h"

#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Interfaces/IHttpResponse.h"

FFileApi::FFileApi()
{
}

FFileApi::~FFileApi()
{
}

void FFileApi::LoadFileFromUrl(const FString& URL, const FString& AssetType)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileApi::FileRequestComplete, AssetType);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

void FFileApi::FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString AssetType)
{
	FString URL = Request->GetURL();
	FString FileName;
	if(URL.Contains(".glb") || URL.Contains(".png"))
	{
		FileName = FPaths::GetCleanFilename(Request->GetURL());
	}
	
	if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
	{
		TArray<uint8> Content = Response->GetContent();
		OnFileRequestComplete.ExecuteIfBound(&Content, FileName, AssetType);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load file from URL"));
	OnFileRequestComplete.ExecuteIfBound(nullptr, FileName, AssetType);
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
