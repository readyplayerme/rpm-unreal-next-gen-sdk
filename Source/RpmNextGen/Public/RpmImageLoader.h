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

	void LoadImageFromURL(UImage* Image, const FString& URL);
	void LoadImageFromURL(TSharedPtr<SImage> ImageWidget, const FString& URL);

private:
	void OnImageDownloadedForUImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TWeakObjectPtr<UImage> Image);
	void OnImageDownloadedForSImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TSharedPtr<SImage> ImageWidget);
	UTexture2D* CreateTextureFromImageData(const TArray<uint8>& ImageData);
};