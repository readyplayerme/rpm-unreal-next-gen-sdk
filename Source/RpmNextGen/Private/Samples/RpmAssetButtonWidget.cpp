// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmAssetButtonWidget.h"
#include "RpmImageLoader.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"

void URpmAssetButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (AssetButton)
    {
        AssetButton->OnClicked.AddDynamic(this, &URpmAssetButtonWidget::HandleButtonClicked);
    }
}

void URpmAssetButtonWidget::InitializeButton(const FAsset& InAssetData, const FVector2D& InImageSize)
{
    AssetData = InAssetData;

    if (AssetImage)
    {
        // Set the image size
        AssetImage->SetDesiredSizeOverride(InImageSize);

        FRpmImageLoader ImageLoader;
        ImageLoader.LoadImageFromURL(AssetImage, AssetData.IconUrl);
    }

    if (SelectionBorder)
    {
        SelectionBorder->SetBrushColor(SelectedColor);
        SelectionBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void URpmAssetButtonWidget::HandleButtonClicked() 
{
    const bool bIsSelected = SelectionBorder->Visibility == ESlateVisibility::Visible;
    SetSelected(!bIsSelected);
    OnAssetButtonClicked.Broadcast(AssetData);
}

void URpmAssetButtonWidget::SetSelected(bool bIsSelected)
{
    if (SelectionBorder)
    {
        if (bIsSelected)
        {
            SelectionBorder->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            SelectionBorder->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}