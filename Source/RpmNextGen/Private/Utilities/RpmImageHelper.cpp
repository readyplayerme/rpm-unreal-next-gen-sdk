#include "Utilities/RpmImageHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "RpmNextGen.h"
#include "Components/Image.h"
#include "Widgets/Images/SImage.h"

UTexture2D* FRpmImageHelper::CreateTextureFromData(const TArray<uint8>& ImageData)
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

UImage* FRpmImageHelper::CreateUImageFromData(const TArray<uint8>& ImageData, const FVector2D& ImageSize)
{
	UImage* Image = NewObject<UImage>();
	if (UTexture2D* Texture = CreateTextureFromData(ImageData))
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		Brush.ImageSize = ImageSize;
		Image->SetBrush(Brush);
		return Image;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load image data to UImage"));
	return Image;
}

void FRpmImageHelper::LoadDataToUImage(const TArray<uint8>& ImageData, UImage*& Image)
{
	if (UTexture2D* Texture = CreateTextureFromData(ImageData))
	{
		LoadTextureToUImage(Texture, Image->Brush.ImageSize, Image);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load data to Texture"));
}

void FRpmImageHelper::LoadDataToSImage(const TArray<uint8>& ImageData, const FVector2D& ImageSize, TSharedPtr<SImage> ImageWidget)
{
	if (ImageWidget.IsValid())
	{
		UTexture2D* Texture = CreateTextureFromData(ImageData);
		LoadTextureToSImage(Texture, ImageSize, ImageWidget);
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load data to Texture"));
}

void FRpmImageHelper::LoadTextureToSImage(UTexture2D* Texture, const FVector2D& ImageSize, TSharedPtr<SImage> ImageWidget)
{
	if (ImageWidget.IsValid())
	{
		FSlateBrush* Brush = new FSlateBrush();
		Brush->SetResourceObject(Texture);
		Brush->ImageSize = ImageSize;
		ImageWidget->SetImage(Brush);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load texture to SImage"));
}

void FRpmImageHelper::LoadTextureToUImage(UTexture2D* Texture, const FVector2D& ImageSize, UImage*& Image)
{
	if (Image)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		Brush.ImageSize = ImageSize;
		Image->SetBrush(Brush);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load texture to UImage"));
}
