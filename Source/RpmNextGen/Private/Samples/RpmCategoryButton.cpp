// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmCategoryButton.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"


void URpmCategoryButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (CategoryButton)
	{
		CategoryButton->OnClicked.AddDynamic(this, &URpmCategoryButton::HandleButtonClicked);
	}

	if (SelectionBorder)
	{
		SelectionBorder->SetBrushColor(FLinearColor::Transparent);
	}
	if (CategoryImageTexture)
	{
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}
}

void URpmCategoryButton::Initialize(FString Category, UTexture2D* Image)
{
	AssetCategoryName = Category;

	if (CategoryImage)
	{
		CategoryImageTexture = Image;
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}	
}

void URpmCategoryButton::SetSelected(bool bInIsSelected)
{
	bIsSelected = bInIsSelected;

	if (SelectionBorder)
	{
		const FLinearColor NewColor = bInIsSelected ? SelectedColor : FLinearColor::Transparent;
		SelectionBorder->SetBrushColor(NewColor);
	}
}

void URpmCategoryButton::HandleButtonClicked()
{
	SetSelected(!bIsSelected);

	OnCategoryClicked.Broadcast(AssetCategoryName);
}

#if WITH_EDITOR
void URpmCategoryButton::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(URpmCategoryButton, CategoryImageTexture))
	{
		// Apply the texture to the CategoryImage widget
		if (CategoryImage && CategoryImageTexture)
		{
			CategoryImage->SetBrushFromTexture(CategoryImageTexture);
			SynchronizeProperties();
		}
		
	}
}
#endif

void URpmCategoryButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (CategoryImage && CategoryImageTexture)
	{
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}
}
