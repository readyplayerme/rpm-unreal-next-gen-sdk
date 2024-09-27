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

void URpmCategoryButtonWidget::InitializeButton(FString Category, UTexture2D* Image, const FVector2D& InImageSize)
{
	AssetCategoryName = Category;

	if (CategoryImage)
	{		
		CategoryImage->SetDesiredSizeOverride(InImageSize);

		if(Image)
		{
			CategoryImageTexture = Image;
			CategoryImage->SetBrushFromTexture(CategoryImageTexture);
			UE_LOG( LogTemp, Warning, TEXT("Setting image on button for Category: %s"), *Category );
		}
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
// This function is called when a property is changed in the editor, it enables the CategoryImageTexture to be updated in the editor
void URpmCategoryButtonWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// If the property that was changed is the CategoryImageTexture apply the new texture to the CategoryImage
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

// This is also required to update the CategoryImageTexture in the editor
void URpmCategoryButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (CategoryImage && CategoryImageTexture)
	{
		CategoryImage->SetBrushFromTexture(CategoryImageTexture);
	}
}
