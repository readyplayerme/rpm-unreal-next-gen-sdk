// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmAssetButtonWidget.h"
#include "RpmImageLoader.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void URpmAssetButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (AssetButton)
    {
        AssetButton->OnClicked.AddDynamic(this, &URpmAssetButtonWidget::HandleButtonClicked);
        DefaultColor = AssetButton->BackgroundColor;
    }
}

void URpmAssetButtonWidget::InitializeButton(const FAsset& InAssetData, const FVector2D& InImageSize)
{    
    AssetData = InAssetData;

    if (AssetImage)
    {
        AssetImage->SetDesiredSizeOverride(InImageSize);

        FRpmImageLoader ImageLoader;
        ImageLoader.LoadImageFromURL(AssetImage, AssetData.IconUrl);
    }
}

void URpmAssetButtonWidget::HandleButtonClicked() 
{
    SetSelected(!bIsSelected);
    OnAssetButtonClicked.Broadcast(this);
}

void URpmAssetButtonWidget::SetSelected(const bool bInIsSelected)
{
    bIsSelected = bInIsSelected;

    if (AssetButton)
    {
        const FLinearColor NewColor = bInIsSelected ? SelectedColor : DefaultColor;
        AssetButton->SetBackgroundColor(NewColor);
    }
}
