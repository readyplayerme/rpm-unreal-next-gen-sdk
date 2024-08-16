// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmCategoryButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"


void URpmCategoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CategoryButton)
	{
		CategoryButton->OnClicked.AddDynamic(this, &URpmCategoryButtonWidget::HandleButtonClicked);
		DefaultColor = CategoryButton->BackgroundColor;
	}
	if (CategoryImageTexture)
	{
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}
}

void URpmCategoryButtonWidget::InitializeButton(FString Category, UTexture2D* Image)
{
	AssetCategoryName = Category;

	if (CategoryImage)
	{
		CategoryImageTexture = Image;
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}	
}

void URpmCategoryButtonWidget::SetSelected(bool bIsSelected)
{
	if (CategoryButton)
	{
		const FLinearColor NewColor = bIsSelected ? SelectedColor : DefaultColor;
		CategoryButton->SetBackgroundColor(NewColor);
	}
}

void URpmCategoryButtonWidget::HandleButtonClicked()
{
	SetSelected(true);

	OnCategoryClicked.Broadcast(this);
}

#if WITH_EDITOR
void URpmCategoryButtonWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(URpmCategoryButtonWidget, CategoryImageTexture))
	{
		if (CategoryImage && CategoryImageTexture)
		{
			CategoryImage->SetBrushFromTexture(CategoryImageTexture);
			SynchronizeProperties();
		}
		
	}
}
#endif

void URpmCategoryButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (CategoryImage && CategoryImageTexture)
	{
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}
}
