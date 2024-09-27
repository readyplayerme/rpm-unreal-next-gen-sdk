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
				return;
			}
			UE_LOG(LogTemp, Error, TEXT("Failed to Load %s button"), *AssetType);
		}
	}
}

void URpmCategoryPanelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void URpmCategoryPanelWidget::AssetTypesLoaded(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful)
{	
	if(bWasSuccessful && ButtonContainer)
	{
		ButtonContainer->ClearChildren();

		for (const FString& AssetType : AssetTypeListResponse.Data)
		{
			CreateButton(AssetType);
		}
		OnCategoriesLoaded.Broadcast(AssetTypeListResponse.Data);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load asset types"));
	OnCategoriesLoaded.Broadcast(TArray<FString>());
}

