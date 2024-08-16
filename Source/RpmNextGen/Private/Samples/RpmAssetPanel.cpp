// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmAssetPanel.h"

#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Components/PanelWidget.h"
#include "Samples/RpmAssetButtonWidget.h"
#include "Settings/RpmDeveloperSettings.h"

void URpmAssetPanel::NativeConstruct()
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
	
	AssetApi->OnListAssetsResponse.BindUObject(this, &URpmAssetPanel::OnAssetListResponse);
}

void URpmAssetPanel::OnAssetListResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse.Data.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Asset Fetch Success."));

		CreateButtonsFromAssets(AssetListResponse.Data);

		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to fetch assets"));
}

void URpmAssetPanel::CreateButtonsFromAssets(TArray<FAsset> Assets)
{
	for (auto Asset : Assets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating button."));
		CreateButton(Asset);
	}
}

void URpmAssetPanel::ClearAllButtons()
{
	if(!AssetButtons.IsEmpty())
	{
		AssetButtons.Empty();
	}
	SelectedAssetButton = nullptr;
}

void URpmAssetPanel::UpdateSelectedButton(URpmAssetButtonWidget* AssetButton)
{
	if(SelectedAssetButton && SelectedAssetButton != AssetButton)
	{
		SelectedAssetButton->SetSelected(false);

	}
	SelectedAssetButton = AssetButton;
}

void URpmAssetPanel::CreateButton(const FAsset& AssetData)
{
	if (AssetButtonBlueprint)
	{
		// Get the current world context
		UWorld* World = GetWorld(); // or whatever provides your current world context
        
		if (World)
		{
			// Create the widget instance
			URpmAssetButtonWidget* AssetButtonInstance = CreateWidget<URpmAssetButtonWidget>(World, AssetButtonBlueprint->GetClass());

			if (AssetButtonInstance)
			{
				// Do something with the instance, e.g., add it to a parent widget or initialize it
				AssetButtonInstance->InitializeButton(AssetData, ImageSize); // Pass any required parameters here

				// If you have a UMG parent widget:
				if(ButtonContainer)
				{
					ButtonContainer->AddChild(AssetButtonInstance);
				}
				UE_LOG(LogTemp, Warning, TEXT("Button created and added."));
				AssetButtons.Add(AssetButtonInstance->GetClass());
				AssetButtonInstance->OnAssetButtonClicked.AddDynamic(this, &URpmAssetPanel::OnAssetButtonClicked);
			}
		}
	}
}


void URpmAssetPanel::OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButton)
{
	UpdateSelectedButton(const_cast<URpmAssetButtonWidget*>(AssetButton));
	OnAssetSelected.Broadcast(AssetButton->GetAssetData());
}

void URpmAssetPanel::LoadAssetsOfType(const FString& AssetType)
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetListQueryParams QueryParams;
	QueryParams.Type = AssetType;
	QueryParams.ApplicationId = Settings->ApplicationId;
	FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);
	AssetApi->ListAssetsAsync(AssetListRequest);	
}
