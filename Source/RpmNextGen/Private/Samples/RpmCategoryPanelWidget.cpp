// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"

#include "Samples/RpmCategoryButton.h"

void URpmCategoryPanelWidget::OnCategoryButtonClicked(URpmCategoryButton* CategoryButton)
{
	UpdateSelectedButton(CategoryButton);
	OnCategorySelected.Broadcast(CategoryButton->AssetCategoryName);
}

void URpmCategoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(CategoryButtons.Num() > 0)
	{
		for(auto CategoryButton : CategoryButtons)
		{
			if(CategoryButton == nullptr)
			{
				continue;
			}
			URpmCategoryButton* Button = CreateWidget<URpmCategoryButton>(this, CategoryButton->GetClass());
			Button->InitializeButton(Button->AssetCategoryName, Button->CategoryImageTexture);
			Button->OnCategoryClicked.AddDynamic(this, &URpmCategoryPanelWidget::OnCategoryButtonClicked);
		}
	}
}

void URpmCategoryPanelWidget::UpdateSelectedButton(URpmCategoryButton* CategoryButton)
{
	if(SelectedCategoryButton)
	{
		SelectedCategoryButton->SetSelected(false);
	}
	SelectedCategoryButton = CategoryButton;
}
