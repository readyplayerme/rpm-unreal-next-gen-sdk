// Fill out your copyright notice in the Description page of Project Settings.


#include "Samples/RpmPaginatorWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void URpmPaginatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (PreviousButton)
	{
		PreviousButton->OnClicked.AddDynamic(this, &URpmPaginatorWidget::OnPrevButtonClicked);
	}

	if (NextButton)
	{
		NextButton->OnClicked.AddDynamic(this, &URpmPaginatorWidget::OnNextButtonClicked);
	}
	
	SetButtons();
}

void URpmPaginatorWidget::Initialize(const FPagination& InPagination)
{
	Pagination = InPagination;
	
	SetButtons();
}

FText URpmPaginatorWidget::GetPageCountText()
{
	return FText::FromString(FString::Printf(TEXT("%d / %d"), Pagination.Page, Pagination.TotalPages));
}

void URpmPaginatorWidget::SetButtons()
{
	if (PageText)
	{
		PageText->SetText(GetPageCountText());
	}

	if (PreviousButton)
	{
		PreviousButton->SetIsEnabled(Pagination.Page > 1);
	}

	if (NextButton)
	{
		NextButton->SetIsEnabled(Pagination.Page < Pagination.TotalPages);
	}
}

void URpmPaginatorWidget::OnPrevButtonClicked()
{
	ChangePage(-1);
}

void URpmPaginatorWidget::OnNextButtonClicked()
{
	ChangePage(1);
}

void URpmPaginatorWidget::ChangePage(const int32 Index)
{
	Pagination.Page = FMath::Clamp(Pagination.Page + Index, 1, Pagination.TotalPages);
	SetButtons();
	
	OnPageChanged.Broadcast(Pagination);
}