// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/AssetButtonWidget.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Async/Async.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UAssetButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (AssetButton)
    {
        AssetButton->OnClicked.AddDynamic(this, &UAssetButtonWidget::HandleButtonClicked);
    }
}

void UAssetButtonWidget::InitializeButton(const FAsset& InAssetData, const FVector2D& InImageSize)
{
    AssetData = InAssetData;

    if (AssetImage)
    {
        // Set the image size
        AssetImage->SetDesiredSizeOverride(InImageSize);

        // Set the image from URL
        SetImageFromURL(AssetImage, AssetData.IconUrl);
    }
}

void UAssetButtonWidget::HandleButtonClicked() 
{
    OnAssetButtonClicked.Broadcast(AssetData);
}

void UAssetButtonWidget::SetImageFromURL(UImage* Image, const FString& URL)
{
    if (!Image)
    {
        UE_LOG(LogTemp, Error, TEXT("SetImageFromURL: Image is null"));
        return;
    }

    TWeakObjectPtr<UImage> WeakImagePtr(Image);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAssetButtonWidget::OnImageDownloaded, WeakImagePtr);
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("GET");
    HttpRequest->ProcessRequest();
}

void UAssetButtonWidget::OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TWeakObjectPtr<UImage> Image)
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

                        // Set the brush from the texture
                        FSlateBrush Brush;
                        Brush.SetResourceObject(Texture);
                        Brush.ImageSize = Image->Brush.ImageSize; // Use the same size as the Image
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