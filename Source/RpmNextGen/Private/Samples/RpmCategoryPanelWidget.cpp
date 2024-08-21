// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Samples/RpmCategoryButtonWidget.h"

void URpmCategoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeCategoryButtons();
}

void URpmCategoryPanelWidget::InitializeCategoryButtons()
{
	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);

	for (UWidget* Widget : Widgets)
	{
		if (URpmCategoryButtonWidget* CategoryButton = Cast<URpmCategoryButtonWidget>(Widget))
		{
			CategoryButton->InitializeButton(CategoryButton->AssetCategoryName, CategoryButton->CategoryImageTexture);
			CategoryButton->OnCategoryClicked.AddDynamic(this, &URpmCategoryPanelWidget::OnCategoryButtonClicked);
		}
	}
}

void URpmCategoryPanelWidget::UpdateSelectedButton(URpmCategoryButtonWidget* CategoryButton)
{
	if(SelectedCategoryButton && SelectedCategoryButton != CategoryButton)
	{
		SelectedCategoryButton->SetSelected(false);
	}
	SelectedCategoryButton = CategoryButton;
}

void URpmCategoryPanelWidget::OnCategoryButtonClicked(URpmCategoryButtonWidget* CategoryButton)
{
	UpdateSelectedButton(CategoryButton);
	OnCategorySelected.Broadcast(CategoryButton->AssetCategoryName);
}

