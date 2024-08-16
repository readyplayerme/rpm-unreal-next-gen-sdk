// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmAssetPanelWidget.h"

#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Samples/RpmAssetButtonWidget.h"
#include "Settings/RpmDeveloperSettings.h"

void URpmAssetPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	AssetApi = MakeShared<FAssetApi>();
	// TODO -  add smarter setting of auth strategy
	if(Settings->ApiProxyUrl.IsEmpty() && !Settings->ApiKey.IsEmpty())
	{
		AssetApi->SetAuthenticationStrategy(new FApiKeyAuthStrategy());
		UE_LOG(LogTemp, Warning, TEXT("Adding ApiKeyAuthStrategy"));
	}
	
	AssetApi->OnListAssetsResponse.BindUObject(this, &URpmAssetPanelWidget::OnAssetListResponse);

	ButtonSize = FVector2D(200, 200);
	ImageSize = FVector2D(200, 200);
}

void URpmAssetPanelWidget::OnAssetListResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse.Data.Num() > 0)
	{
		CreateButtonsFromAssets(AssetListResponse.Data);

		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to fetch assets"));
}

void URpmAssetPanelWidget::CreateButtonsFromAssets(TArray<FAsset> Assets)
{
	for (auto Asset : Assets)
	{
		CreateButton(Asset);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("No assets found") );
}

void URpmAssetPanelWidget::ClearAllButtons()
{
	if(!AssetButtons.IsEmpty())
	{
		AssetButtons.Empty();
	}
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
		UWorld* World = GetWorld();
        
		if (World)
		{
			URpmAssetButtonWidget* AssetButtonInstance = CreateWidget<URpmAssetButtonWidget>(World, AssetButtonBlueprint);

			if (AssetButtonInstance)
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

				AssetButtons.Add(AssetButtonBlueprint);

				AssetButtonInstance->OnAssetButtonClicked.AddDynamic(this, &URpmAssetPanelWidget::OnAssetButtonClicked);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AssetButtonBlueprint is not set!"));
	}
}



void URpmAssetPanelWidget::OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButton)
{
	UpdateSelectedButton(const_cast<URpmAssetButtonWidget*>(AssetButton));
	OnAssetSelected.Broadcast(AssetButton->GetAssetData());
}

void URpmAssetPanelWidget::LoadAssetsOfType(const FString& AssetType)
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetListQueryParams QueryParams;
	QueryParams.Type = AssetType;
	QueryParams.ApplicationId = Settings->ApplicationId;
	FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);
	AssetApi->ListAssetsAsync(AssetListRequest);	
}
