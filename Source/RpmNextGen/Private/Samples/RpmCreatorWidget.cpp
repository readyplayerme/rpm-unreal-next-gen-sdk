// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCreatorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Samples/RpmAssetPanelWidget.h"

class UVerticalBox;

void URpmCreatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URpmCreatorWidget::CreateAssetPanelsFromCategories(const TArray<FString>& CategoryArray)
{
	if (CategoryArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No categories to create asset panels from!"));
		return;
	}
	if (!AssetPanelSwitcher || !AssetPanelBlueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetSwitcher or WidgetBlueprintClass is not set!"));
		return;
	}

	AssetPanelSwitcher->ClearChildren();

	IndexMapByCategory.Empty();
	for (int i = 0; i < CategoryArray.Num(); ++i)
	{
		CreateAssetPanel(CategoryArray[i]);
		IndexMapByCategory.Add(CategoryArray[i],  i );
	}
	if(IndexMapByCategory.Num() > 0)
	{
		SwitchToPanel(CategoryArray[0]);
	}
}

void URpmCreatorWidget::SwitchToPanel(const FString& Category)
{
	if(AssetPanelSwitcher)
	{
		if(IndexMapByCategory.Num() < 1 || IndexMapByCategory[Category] == -1)
		{
			UE_LOG(LogTemp, Error, TEXT("Category %s not found! Category length %d"), *Category, IndexMapByCategory.Num());
			return;
		}
		AssetPanelSwitcher->SetActiveWidgetIndex(IndexMapByCategory[Category]);
	}
}

void URpmCreatorWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void URpmCreatorWidget::HandleAssetSelectedFromPanel(const FAsset& AssetData)
{
	OnAssetSelected.Broadcast(AssetData);
}

UUserWidget* URpmCreatorWidget::CreateAssetPanel(const FString& Category)
{
	if (!AssetPanelBlueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetBlueprintClass is not set!"));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return nullptr;
	}

	URpmAssetPanelWidget* AssetPanelWidget = CreateWidget<URpmAssetPanelWidget>(World, AssetPanelBlueprint);

	if (!AssetPanelWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create widget from blueprint class!"));
		return nullptr;
	}
	AssetPanelSwitcher->AddChild(AssetPanelWidget);
	AssetPanelWidget->PaginationLimit = PaginationLimit;
	AssetPanelWidget->Rename(*Category);
	AssetPanelWidget->SetCategoryName(Category);
	AssetPanelWidget->ButtonSize = FVector2D(200, 200);
	AssetPanelWidget->ImageSize = FVector2D(200, 200);
	AssetPanelWidget->OnAssetSelected.AddDynamic(this, &URpmCreatorWidget::HandleAssetSelectedFromPanel);
	AssetPanelWidget->LoadAssetsOfType(Category);
	return AssetPanelWidget;
}
