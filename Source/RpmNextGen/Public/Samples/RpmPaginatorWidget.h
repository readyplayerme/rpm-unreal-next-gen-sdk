// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Common/Models/Pagination.h"
#include "Blueprint/UserWidget.h"
#include "RpmPaginatorWidget.generated.h"

class UTextBlock;
class UButton;
struct FPagination;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPreviousButtonClicked);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmPaginatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnNextButtonClicked OnNextButtonEvent;

	UPROPERTY( BlueprintAssignable, Category = "Events" )
	FOnPreviousButtonClicked OnPreviousButtonEvent;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me" )
	void UpdateState(const FPagination& Pagination);

	UFUNCTION()
	void OnPrevButtonClicked();
	UFUNCTION()
	void OnNextButtonClicked();

protected:
	
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* PreviousButton;

	UPROPERTY(meta = (BindWidget))
	UButton* NextButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PageText;

	FText GetPageCountText(const FPagination& Pagination);
};
