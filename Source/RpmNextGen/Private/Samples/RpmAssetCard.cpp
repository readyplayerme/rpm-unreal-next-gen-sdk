// Fill out your copyright notice in the Description page of Project Settings.

#include "RpmImageLoader.h"
#include "Api/Assets/Models/Asset.h"
#include "Components/TextBlock.h"
#include "Samples/RpmAssetCardWidget.h"

void URpmAssetCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void URpmAssetCardWidget::InitializeCard(const FAsset& Asset)
{
	AssetData = Asset;
	AssetCategoryText->SetText(FText::FromString(AssetData.Type));
	AssetNameText->SetText(FText::FromString(AssetData.Name));
	AssetIdText->SetText(FText::FromString(AssetData.Id));
	LoadImage(AssetData.IconUrl);
}

void URpmAssetCardWidget::LoadImage(const FString& URL)
{
	FRpmImageLoader ImageLoader;
	ImageLoader.LoadImageFromURL(AssetImage, URL);
}

