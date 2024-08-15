// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmAssetPanel.h"

#include "Components/PanelWidget.h"
#include "Samples/RpmAssetButtonWidget.h"

void URpmAssetPanel::LoadAssetsAndCreateButtons(TArray<FAsset> Assets)
{
	for (auto Asset : Assets)
	{
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

void URpmAssetPanel::UpdateSelectedButton(TSubclassOf<URpmAssetButtonWidget> AssetButton)
{
	if(SelectedAssetButton)
	{
		SelectedAssetButton.GetDefaultObject()->SetSelected(false);
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
			URpmAssetButtonWidget* AssetButtonInstance = CreateWidget<URpmAssetButtonWidget>(World, AssetButtonBlueprint);

			if (AssetButtonInstance)
			{
				// Do something with the instance, e.g., add it to a parent widget or initialize it
				AssetButtonInstance->InitializeButton(AssetData, ImageSize); // Pass any required parameters here

				// If you have a UMG parent widget:
				if(ButtonContainer)
				{
					ButtonContainer->AddChild(AssetButtonInstance);
				}

				AssetButtons.Add(AssetButtonInstance->GetClass());
				AssetButtonInstance->OnAssetButtonClicked.AddDynamic(this, &URpmAssetPanel::OnAssetButtonClicked);
			}
		}
	}
}

void URpmAssetPanel::OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButton)
{
	OnAssetSelected.Broadcast(AssetButton->GetAssetData());
}