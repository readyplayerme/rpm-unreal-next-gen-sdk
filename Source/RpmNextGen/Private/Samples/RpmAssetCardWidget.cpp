// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmAssetCardWidget.h"
#include "RpmImageLoader.h"
#include "Api/Assets/Models/Asset.h"
#include "Components/TextBlock.h"

void URpmAssetCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	this->SetVisibility(ESlateVisibility::Hidden);
}

void URpmAssetCardWidget::InitializeCard(const FAsset& Asset)
{
	this->SetVisibility(ESlateVisibility::Visible);
	AssetData = Asset;
	
	AssetCategoryText->SetText(FText::FromString(AssetData.Type));
	AssetNameText->SetText(FText::FromString(AssetData.Name));
	AssetNameText->SetVisibility( AssetData.Name.IsEmpty() ? ESlateVisibility::Hidden : ESlateVisibility::Visible );
	AssetIdText->SetText(FText::FromString(AssetData.Id));
	
	LoadImage(AssetData.IconUrl);
}

void URpmAssetCardWidget::LoadImage(const FString& URL)
{
	FRpmImageLoader ImageLoader;
	ImageLoader.LoadImageFromURL(AssetImage, URL);
}

