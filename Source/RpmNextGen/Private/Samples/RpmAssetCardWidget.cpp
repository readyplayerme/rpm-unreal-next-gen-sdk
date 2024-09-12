// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmAssetCardWidget.h"
#include "RpmTextureLoader.h"
#include "Api/Assets/Models/Asset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Utilities/RpmImageHelper.h"

void URpmAssetCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	this->SetVisibility(ESlateVisibility::Hidden);
	if(TextureLoader.IsValid())
	{
		TextureLoader = MakeShared<FRpmTextureLoader>();
		TextureLoader->OnTextureLoaded.BindUObject(this, &URpmAssetCardWidget::OnTextureLoaded);
	}
}

void URpmAssetCardWidget::InitializeCard(const FAsset& Asset)
{
	this->SetVisibility(ESlateVisibility::Visible);
	AssetData = Asset;
	
	AssetCategoryText->SetText(FText::FromString(AssetData.Type));
	AssetNameText->SetText(FText::FromString(AssetData.Name));
	AssetNameText->SetVisibility( AssetData.Name.IsEmpty() ? ESlateVisibility::Hidden : ESlateVisibility::Visible );
	AssetIdText->SetText(FText::FromString(AssetData.Id));
	
	LoadImage(AssetData);
}

void URpmAssetCardWidget::LoadImage(const FAsset& Asset)
{
	AssetData = Asset;
	TextureLoader->LoadIconFromAsset(Asset);
}

void URpmAssetCardWidget::OnTextureLoaded(UTexture2D* Texture2D)
{
	FRpmImageHelper::LoadTextureToUImage(Texture2D, AssetImage->Brush.ImageSize, AssetImage);
}


