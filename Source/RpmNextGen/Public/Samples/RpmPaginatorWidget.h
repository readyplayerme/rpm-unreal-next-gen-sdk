// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Common/Models/Pagination.h"
#include "Blueprint/UserWidget.h"
#include "RpmPaginatorWidget.generated.h"

class UTextBlock;
class UButton;
struct FPagination;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPageChanged, const FPagination&, Pargination);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmPaginatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnPageChanged OnPageChanged;
	
	void Initialize(const FPagination& InPagination);
	FText GetPageCountText();

	UFUNCTION()
	void OnPrevButtonClicked();
	UFUNCTION()
	void OnNextButtonClicked();

protected:
	FPagination Pagination;
	
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* PreviousButton;

	UPROPERTY(meta = (BindWidget))
	UButton* NextButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PageText;
	
	void SetButtons();
	void ChangePage(int32 Index);
};
