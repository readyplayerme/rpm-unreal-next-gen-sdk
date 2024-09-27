// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/AssetTypeListResponse.h"
#include "Blueprint/UserWidget.h"
#include "RpmCategoryPanelWidget.generated.h"

class URpmAssetButtonWidget;
class FAssetApi;
class URpmCategoryButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySelected, const FString&, CategoryName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoriesLoaded, const TArray<FString>, CategoryNames);
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
	TSubclassOf<URpmCategoryButtonWidget> CategoryButtonBlueprint;
	
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ButtonContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Panel", meta = (ExposeOnSpawn = "true") )
	FVector2D ButtonSize;
	
	UPROPERTY()
	URpmCategoryButtonWidget* SelectedCategoryButton;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCategorySelected OnCategorySelected;

	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnCategoriesLoaded OnCategoriesLoaded;
	
	UFUNCTION(BlueprintCallable, Category = "Category Panel")
	virtual void UpdateSelectedButton(URpmCategoryButtonWidget* CategoryButton);

	UFUNCTION(BlueprintCallable, Category = "Category Panel")
	void LoadAndCreateButtons();

	UFUNCTION()
	virtual void OnCategoryButtonClicked(URpmCategoryButtonWidget* CategoryButton);
	
	virtual void CreateButton(const FString& AssetType);

	virtual void SynchronizeProperties() override;
private:
	void AssetTypesLoaded(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful);

	TArray<TSubclassOf<URpmCategoryButtonWidget>> AssetButtons;
	TSharedPtr<FAssetApi> AssetApi;
	
};
