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

void FRpmImageLoader::LoadImageFromURL(UImage* Image, const FString& URL)
{
    if (!Image)
    {
        UE_LOG(LogTemp, Error, TEXT("LoadImageFromURL: Image is null"));
        return;
    }

    TWeakObjectPtr<UImage> WeakImagePtr(Image);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &FRpmImageLoader::OnImageDownloadedForUImage, WeakImagePtr);
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("GET");
    HttpRequest->ProcessRequest();
}

void FRpmImageLoader::LoadImageFromURL(TSharedPtr<SImage> ImageWidget, const FString& URL)
{
    if (!ImageWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("LoadImageFromURL: SImage widget is invalid"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &FRpmImageLoader::OnImageDownloadedForSImage, ImageWidget);
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("GET");
    HttpRequest->ProcessRequest();
}

void FRpmImageLoader::OnImageDownloadedForUImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TWeakObjectPtr<UImage> Image)
{
    if (!Image.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("OnImageDownloaded: Image is null or no longer valid"));
        return;
    }

    if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
    {
        TArray<uint8> ImageData = Response->GetContent();
        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
        if (ImageFormat != EImageFormat::Invalid)
        {
            TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

            if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
            {
                TArray<uint8> UncompressedBGRA;
                if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
                {
                    UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
                    if (Texture)
                    {
                        void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
                        FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
                        Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

                        Texture->UpdateResource();

                        FSlateBrush Brush;
                        Brush.SetResourceObject(Texture);
                        Brush.ImageSize = Image->Brush.ImageSize;
                        AsyncTask(ENamedThreads::GameThread, [Image, Brush]() {
                            if (Image.IsValid())
                            {
                                Image->SetBrush(Brush);
                            }
                        });
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Unsupported image format: %s"), *Request->GetURL());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to download image from URL: %s"), *Request->GetURL());
    }
}

void FRpmImageLoader::OnImageDownloadedForSImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TSharedPtr<SImage> ImageWidget)
{
    if (!ImageWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("OnImageDownloadedForSImage: SImage widget is invalid"));
        return;
    }

    UTexture2D* Texture = nullptr;
    if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
    {
        Texture = CreateTextureFromImageData(Response->GetContent());
    }

    if (Texture)
    {
        FSlateBrush* Brush = new FSlateBrush();
        Brush->SetResourceObject(Texture);
        Brush->ImageSize = FVector2D(100.0f, 100.0f);  // You can adjust the size as needed

        AsyncTask(ENamedThreads::GameThread, [ImageWidget, Brush]() {
            ImageWidget->SetImage(Brush);
        });
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to download or create texture from URL: %s"), *Request->GetURL());
    }
}

UTexture2D* FRpmImageLoader::CreateTextureFromImageData(const TArray<uint8>& ImageData)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());

    if (ImageFormat != EImageFormat::Invalid)
    {
        TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
        if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
        {
            TArray<uint8> UncompressedBGRA;
            if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
            {
                UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
                if (Texture)
                {
                    void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
                    FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
                    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

                    Texture->UpdateResource();
                    return Texture;
                }
            }
        }
    }

    return nullptr;
}

