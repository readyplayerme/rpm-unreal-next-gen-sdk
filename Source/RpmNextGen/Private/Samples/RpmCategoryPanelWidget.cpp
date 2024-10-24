// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"
#include "RpmNextGen.h"
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
	if(bIsInitialized)
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("URpmCategoryPanelWidget Already initialized"));
		return;
	}
	bIsInitialized = true;
	AssetApi = MakeShared<FAssetApi>();
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
	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
	TSharedPtr<FAssetTypeListRequest> AssetTypeListRequest = MakeShared<FAssetTypeListRequest>();
	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	AssetTypeListRequest->Params = QueryParams;
	AssetApi->ListAssetTypesAsync(AssetTypeListRequest, FOnListAssetTypesResponse::CreateUObject(this, &URpmCategoryPanelWidget::AssetTypesLoaded));
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
		const auto CleanAssetType = AssetType.Replace(TEXT(" "), TEXT("-"));
		if(UObject* LoadedAsset = StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString::Printf(TEXT("/RpmNextGen/Samples/Icons/T-rpm-%s"), *CleanAssetType)))
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
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to Load %s button"), *AssetType);
		}
	}
}

void URpmCategoryPanelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void URpmCategoryPanelWidget::AssetTypesLoaded(TSharedPtr<FAssetTypeListResponse> Response, bool bWasSuccessful)
{	
	if(bWasSuccessful && ButtonContainer && Response.IsValid())
	{
		ButtonContainer->ClearChildren();

		for (const FString& AssetType : Response->Data)
		{
			CreateButton(AssetType);
		}
		OnCategoriesLoaded.Broadcast(Response->Data);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load asset types"));
	OnCategoriesLoaded.Broadcast(TArray<FString>());
}

