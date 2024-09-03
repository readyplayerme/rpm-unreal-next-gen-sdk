// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RpmCategoryPanelWidget.generated.h"

class URpmCategoryButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySelected, const FString&, CategoryName);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmCategoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Panel")
	URpmCategoryButtonWidget* SelectedCategoryButton;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCategorySelected OnCategorySelected;
	
	UFUNCTION(BlueprintCallable, Category = "Category Panel")
	virtual void UpdateSelectedButton(URpmCategoryButtonWidget* CategoryButton);

	UFUNCTION()
	virtual void OnCategoryButtonClicked(URpmCategoryButtonWidget* CategoryButton);

private:
	void InitializeCategoryButtons();
};
