// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"
#include "Samples/RpmCategoryButton.h"

void URpmCategoryPanelWidget::UpdateSelectedButton(TSubclassOf<URpmCategoryButton> CategoryButton)
{
	if(SelectedCategoryButton->IsValidLowLevel())
	{
		SelectedCategoryButton.GetDefaultObject()->SetSelected(false);
	}
	SelectedCategoryButton = CategoryButton;
}
