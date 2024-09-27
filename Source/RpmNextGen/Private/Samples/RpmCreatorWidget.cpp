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
	IndexMapByCategory = TMap<FString, int32>();
}

void URpmCreatorWidget::CreateAssetPanelsFromCategories(const TArray<FString>& CategoryNames)
{
	if (!AssetPanelSwitcher || !WidgetBlueprintClass)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetSwitcher or WidgetBlueprintClass is not set!"));
		return;
	}

	AssetPanelSwitcher->ClearChildren();

	IndexMapByCategory.Empty();
	for (int i = 0; i < CategoryNames.Num(); ++i)
	{
		if (UUserWidget* AssetPanelWidget = CreateWidgetFromName(CategoryNames[i]))
		{
			AssetPanelSwitcher->AddChild(AssetPanelWidget);
		}
		IndexMapByCategory.Add(CategoryNames[i],  i );
	}
}

void URpmCreatorWidget::SwitchToPanel(const FString& WidgetName)
{
	if(AssetPanelSwitcher)
	{
		if(IndexMapByCategory[WidgetName] == -1)
		{
			UE_LOG(LogTemp, Error, TEXT("Category %s not found!"), *WidgetName);
			return;
		}
		AssetPanelSwitcher->SetActiveWidgetIndex(IndexMapByCategory[WidgetName]);
	}
}

void URpmCreatorWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

UUserWidget* URpmCreatorWidget::CreateWidgetFromName(const FString& WidgetName)
{
	if (!WidgetBlueprintClass)
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

	URpmAssetPanelWidget* AssetPanelWidget = CreateWidget<URpmAssetPanelWidget>(World, WidgetBlueprintClass);

	if (!AssetPanelWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create widget from blueprint class!"));
		return nullptr;
	}
	AssetPanelWidget->Rename(*WidgetName);
	AssetPanelWidget->SetCategoryName(WidgetName);
	AssetPanelWidget->LoadAssetsOfType(WidgetName);
	return AssetPanelWidget;
}
