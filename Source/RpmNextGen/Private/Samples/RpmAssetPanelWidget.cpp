// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmAssetPanelWidget.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Cache/AssetCacheManager.h"
#include "Cache/CachedAssetData.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Samples/RpmAssetButtonWidget.h"
#include "Settings/RpmDeveloperSettings.h"

void URpmAssetPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AssetApi = MakeShared<FAssetApi>();
}

void URpmAssetPanelWidget::OnAssetListResponse(TSharedPtr<FAssetListResponse> Response, bool bWasSuccessful)
{
	if(bWasSuccessful && Response->Data.Num() > 0)
	{
		Pagination = Response->Pagination;
		OnPaginationUpdated.Broadcast(Pagination);
		CreateButtonsFromAssets(Response->Data);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch assets"));
}

void URpmAssetPanelWidget::LoadAssetsFromCache(const FString& AssetType)
{
	TArray<FCachedAssetData> CachedAssets = FAssetCacheManager::Get().GetAssetsOfType(AssetType);
	for (auto CachedAsset : CachedAssets)
	{
		CreateButton(CachedAsset.ToAsset());
	}
}

void URpmAssetPanelWidget::CreateButtonsFromAssets(TArray<FAsset> Assets)
{
	if(Assets.Num() < 1)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("No assets found"));
		return;
	}
	for (auto Asset : Assets)
	{
		CreateButton(Asset);
	}
}

void URpmAssetPanelWidget::ClearAllButtons()
{
	if (ButtonContainer)
	{
		ButtonContainer->ClearChildren();
        
		for (auto& ButtonPair : AssetButtonMap)
		{
			if (URpmAssetButtonWidget* ButtonWidget = Cast<URpmAssetButtonWidget>(ButtonPair.Value->GetDefaultObject()))
			{
				ButtonWidget->RemoveFromParent();
				ButtonWidget->ConditionalBeginDestroy(); 
			}
		}
	}

	AssetButtonMap.Empty();
	SelectedAssetButton = nullptr;
}

void URpmAssetPanelWidget::UpdateSelectedButton(URpmAssetButtonWidget* AssetButton)
{
	if(SelectedAssetButton && SelectedAssetButton != AssetButton)
	{
		SelectedAssetButton->SetSelected(false);
	}
	SelectedAssetButton = AssetButton;
}

void URpmAssetPanelWidget::CreateButton(const FAsset& AssetData)
{
	if (AssetButtonBlueprint)
	{
		if (UWorld* World = GetWorld())
		{
			if (URpmAssetButtonWidget* AssetButtonInstance = CreateWidget<URpmAssetButtonWidget>(World, AssetButtonBlueprint))
			{
				USizeBox* ButtonSizeBox = NewObject<USizeBox>(this);
				if (ButtonSizeBox && ButtonContainer)
				{
					ButtonSizeBox->SetWidthOverride(ButtonSize.X);
					ButtonSizeBox->SetHeightOverride(ButtonSize.Y);
					ButtonSizeBox->AddChild(AssetButtonInstance);
					ButtonContainer->AddChild(ButtonSizeBox);
				}
				
				AssetButtonInstance->InitializeButton(AssetData, ImageSize);
				AssetButtonMap.Add(AssetData.Id, AssetButtonBlueprint);
				AssetButtonInstance->OnAssetButtonClicked.AddDynamic(this, &URpmAssetPanelWidget::OnAssetButtonClicked);
			}
		}
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("AssetButtonBlueprint is not set!"));
	}
}

void URpmAssetPanelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void URpmAssetPanelWidget::OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButton)
{
	UpdateSelectedButton(const_cast<URpmAssetButtonWidget*>(AssetButton));
	OnAssetSelected.Broadcast(AssetButton->GetAssetData());
}

void URpmAssetPanelWidget::LoadAssetsOfType(const FString& AssetType)
{
	CurrentAssetType = AssetType;
	if (!AssetApi.IsValid())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("AssetApi is null or invalid"));
		return;
	}
	
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	FAssetListQueryParams QueryParams;
	QueryParams.Type = AssetType;
	QueryParams.ApplicationId = RpmSettings->ApplicationId;
	QueryParams.Limit = PaginationLimit;
	QueryParams.Page = Pagination.Page;
	const TSharedPtr<FAssetListRequest> AssetListRequest = MakeShared<FAssetListRequest>(QueryParams);
	AssetApi->ListAssetsAsync(AssetListRequest,FOnListAssetsResponse::CreateUObject(this, &URpmAssetPanelWidget::OnAssetListResponse));	
}

void URpmAssetPanelWidget::LoadNextPage()
{
	if (!Pagination.HasNextPage)
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Already on the last page"));
		return;
	}
	ClearAllButtons();
	Pagination.Page++;
	LoadAssetsOfType(CurrentAssetType);
}

void URpmAssetPanelWidget::LoadPreviousPage()
{
	if (!Pagination.HasPrevPage)
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Already on the first page"));
		return;
	}
	ClearAllButtons();
	Pagination.Page--;
	LoadAssetsOfType(CurrentAssetType);  
}
