#include "RpmImageLoader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Async/Async.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "Widgets/Images/SImage.h"

void FRpmImageLoader::LoadUImageFromURL(UImage* Image, const FString& URL)
{
	if (!Image)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadImageFromURL: Image is null"));
		return;
	}

	TWeakObjectPtr<UImage> WeakImagePtr(Image);
	DownloadImage(URL, [WeakImagePtr](UTexture2D* Texture)
	{
		if (WeakImagePtr.IsValid() && Texture)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(Texture);
			Brush.ImageSize = WeakImagePtr->Brush.ImageSize;
			WeakImagePtr->SetBrush(Brush);
		}
	});
}

void FRpmImageLoader::LoadSImageFromURL(TSharedPtr<SImage> ImageWidget, const FString& URL,
                                        TFunction<void(UTexture2D*)> OnImageUpdated)
{
	if (!ImageWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("LoadImageFromURL: SImage widget is invalid"));
		return;
	}

	DownloadImage(URL, [ImageWidget, OnImageUpdated](UTexture2D* Texture)
	{
		if (ImageWidget.IsValid() && Texture)
		{
			FSlateBrush* Brush = new FSlateBrush();
			Brush->SetResourceObject(Texture);
			Brush->ImageSize = FVector2D(100.0f, 100.0f); // Adjust size as needed
			ImageWidget->SetImage(Brush);
			OnImageUpdated(Texture);
		}
	});
}

void FRpmImageLoader::DownloadImage(const FString& URL, TFunction<void(UTexture2D*)> OnImageDownloaded)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FRpmImageLoader::OnImageDownloadComplete, OnImageDownloaded);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

void FRpmImageLoader::OnImageDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TFunction<void(UTexture2D*)> OnImageDownloaded)
{
	UTexture2D* Texture = nullptr;
	if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
	{
		Texture = CreateTextureFromImageData(Response->GetContent());
	}

	if (!Texture)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to download or create texture from URL: %s"), *Request->GetURL());
	}

	AsyncTask(ENamedThreads::GameThread, [OnImageDownloaded, Texture]()
	{
		OnImageDownloaded(Texture);
	});
}

UTexture2D* FRpmImageLoader::CreateTextureFromImageData(const TArray<uint8>& ImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(
		FName("ImageWrapper"));
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());

	if (ImageFormat == EImageFormat::Invalid)
	{
		return nullptr;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		return nullptr;
	}

	TArray<uint8> UncompressedBGRA;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
	{
		return nullptr;
	}

	UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
	if (!Texture)
	{
		return nullptr;
	}

	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	return Texture;
}
