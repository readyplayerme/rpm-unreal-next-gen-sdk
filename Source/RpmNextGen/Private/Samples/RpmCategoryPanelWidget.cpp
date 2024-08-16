// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Samples/RpmCategoryButton.h"

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
		if (URpmCategoryButton* CategoryButton = Cast<URpmCategoryButton>(Widget))
		{
			CategoryButton->InitializeButton(CategoryButton->AssetCategoryName, CategoryButton->CategoryImageTexture);
			CategoryButton->OnCategoryClicked.AddDynamic(this, &URpmCategoryPanelWidget::OnCategoryButtonClicked);
		}
	}
}

void URpmCategoryPanelWidget::UpdateSelectedButton(URpmCategoryButton* CategoryButton)
{
	if(SelectedCategoryButton && SelectedCategoryButton != CategoryButton)
	{
		SelectedCategoryButton->SetSelected(false);

	}
	SelectedCategoryButton = CategoryButton;
}

void URpmCategoryPanelWidget::OnCategoryButtonClicked(URpmCategoryButton* CategoryButton)
{
	UpdateSelectedButton(CategoryButton);
	OnCategorySelected.Broadcast(CategoryButton->AssetCategoryName);
}

