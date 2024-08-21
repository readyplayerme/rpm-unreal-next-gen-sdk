#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"

class RPMNEXTGEN_API FRpmImageLoader
{
public:
	FRpmImageLoader() = default;
	void LoadUImageFromURL(UImage* Image, const FString& URL);
	void LoadSImageFromURL(TSharedPtr<SImage> ImageWidget, const FString& URL);

private:
	void DownloadImage(const FString& URL, TFunction<void(UTexture2D*)> OnImageDownloaded);
	void OnImageDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TFunction<void(UTexture2D*)> OnImageDownloaded);
	UTexture2D* CreateTextureFromImageData(const TArray<uint8>& ImageData);
};