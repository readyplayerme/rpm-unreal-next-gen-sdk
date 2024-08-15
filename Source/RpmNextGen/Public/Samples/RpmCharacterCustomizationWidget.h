// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RpmAssetPanel.h"
#include "Api/Assets/Models/Asset.h"
#include "Blueprint/UserWidget.h"
#include "Api/Characters/CharacterApi.h"
#include "RpmCharacterCustomizationWidget.generated.h"

class URpmAssetButtonWidget;
class FCharacterApi;
struct FAssetListResponse;
class FApiKeyAuthStrategy;
class UImage;
class UVerticalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssetsFetchedDelegate, bool, bWasSuccessful, const TArray<FAsset>&, AssetDataArray);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmCharacterCustomizationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void PopulateBox(UPanelWidget* ParentBox);

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void ClearBox(UPanelWidget* ParentBox);

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void UpdateCustomizationOptions(UPanelWidget* ParentBox);

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void PopulateBoxWithFilter(UPanelWidget* ParentBox, const FString& AssetType);

	UPROPERTY(BlueprintAssignable, Category = "Customization")
	FOnAssetsFetchedDelegate OnAssetsFetchedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Customization")
	FOnAssetSelected OnAssetButtonSelected;

	// Property to define the size of the images
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization", meta = (ClampMin = "0.0", UIMin = "0.0", ToolTip = "Recommended to keep this square (e.g., 200x200)"))
	FVector2D ImageSize = FVector2D(200.0f, 200.0f);  // Default size of 200x200

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	TSubclassOf<UUserWidget> AssetButtonWidgetClass;
	
private:
	void InitializeCustomizationOptions();
	void OnAssetsFetched(const FAssetListResponse& AssetListResponse, bool bWasSuccessful);
	void CreateAssetWidget(const FAsset& AssetData, UPanelWidget* ParentBox);
	
	UFUNCTION()
	void OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButton);

	FString ApplicationID;
	TArray<FAsset> AssetDataArray;
};
