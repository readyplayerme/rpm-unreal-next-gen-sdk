// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"

#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Samples/RpmCategoryButtonWidget.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmDeveloperSettings;

void URpmCategoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AssetApi = MakeShared<FAssetApi>();
	AssetApi->OnListAssetTypeResponse.BindUObject(this, &URpmCategoryPanelWidget::AssetTypesLoaded);
	AssetButtons = TArray<TSubclassOf<URpmCategoryButtonWidget>>();
	UE_LOG(LogTemp, Warning, TEXT("ButtonSize at Construct: %s"), *ButtonSize.ToString());
}

void URpmCategoryPanelWidget::UpdateSelectedButton(URpmCategoryButtonWidget* CategoryButton)
{
	if(SelectedCategoryButton && SelectedCategoryButton != CategoryButton)
	{
		SelectedCategoryButton->SetSelected(false);
	}
	SelectedCategoryButton = CategoryButton;
}

void URpmCategoryPanelWidget::LoadAndCreateButtons()
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetTypeListRequest AssetListRequest;
	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	AssetListRequest.Params = QueryParams;
	AssetApi->ListAssetTypesAsync(AssetListRequest);
}

void URpmCategoryPanelWidget::OnCategoryButtonClicked(URpmCategoryButtonWidget* CategoryButton)
{
	UpdateSelectedButton(CategoryButton);
	OnCategorySelected.Broadcast(CategoryButton->AssetCategoryName);
}

void URpmCategoryPanelWidget::CreateButton(const FString& AssetType)
{
	// Log the asset type
	UE_LOG(LogTemp, Warning, TEXT("Asset Type: %s"), *AssetType);


	if (UWorld* World = GetWorld())
	{
		URpmCategoryButtonWidget* CategoryButton = CreateWidget<URpmCategoryButtonWidget>(World, CategoryButtonBlueprint);
		UTexture2D* ButtonTexture = nullptr;
		if(UObject* LoadedAsset = StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString::Printf(TEXT("/RpmNextGenSDK/Content/Samples/BasicUI/Icons/T-rpm-%s"), *AssetType)))
		{
			ButtonTexture = Cast<UTexture2D>(LoadedAsset);
		}
		if (CategoryButton)
		{

			USizeBox* ButtonSizeBox = NewObject<USizeBox>(this);
			if (ButtonSizeBox && ButtonContainer)
			{
				ButtonSizeBox->SetWidthOverride(ButtonSize.X);
				ButtonSizeBox->SetHeightOverride(ButtonSize.Y);
				ButtonSizeBox->AddChild(CategoryButton);
				ButtonContainer->AddChild(ButtonSizeBox);
				CategoryButton->InitializeButton(AssetType, ButtonTexture, ButtonSize);
				CategoryButton->OnCategoryClicked.AddDynamic(this, &URpmCategoryPanelWidget::OnCategoryButtonClicked);
				AssetButtons.Add(CategoryButton->GetClass());
				UE_LOG(LogTemp, Log, TEXT("%s Button created successfully"), *AssetType);
				return;
			}


			UE_LOG(LogTemp, Error, TEXT("Failed to Load %s button"), *AssetType);
		}
	}
}

void URpmCategoryPanelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	UE_LOG(LogTemp, Warning, TEXT("ButtonSize at SynchronizeProperties: %s"), *ButtonSize.ToString());

}

void URpmCategoryPanelWidget::AssetTypesLoaded(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful)
{	
	if(bWasSuccessful && ButtonContainer)
	{
		UE_LOG(LogTemp, Log, TEXT("%d Asset types loaded successfully"), AssetTypeListResponse.Data.Num());
		// Clear any previous buttons from the container
		ButtonContainer->ClearChildren();

		for (const FString& AssetType : AssetTypeListResponse.Data)
		{
			CreateButton(AssetType);
		}
	}
}

