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
	UpdateState(FPagination());
}

void URpmPaginatorWidget::UpdateState(const FPagination& Pagination)
{	
	if (PageText)
	{
		PageText->SetText(GetPageCountText(Pagination));
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

FText URpmPaginatorWidget::GetPageCountText(const FPagination& Pagination)
{
	return FText::FromString(FString::Printf(TEXT("%d / %d"), Pagination.Page, Pagination.TotalPages));
}

void URpmPaginatorWidget::OnPrevButtonClicked()
{
	OnPreviousButtonEvent.Broadcast();
}

void URpmPaginatorWidget::OnNextButtonClicked()
{
	OnNextButtonEvent.Broadcast();
}