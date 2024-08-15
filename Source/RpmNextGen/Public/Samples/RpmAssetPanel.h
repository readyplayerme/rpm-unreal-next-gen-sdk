// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetPanel.generated.h"

struct FAsset;
class URpmAssetButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetSelected, const FAsset&, AssetData);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmAssetPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel" )
	TSubclassOf<URpmAssetButtonWidget> AssetButtonBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel" )
	UPanelWidget* ButtonContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel")
	TSubclassOf<URpmAssetButtonWidget> SelectedAssetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	FVector2D ImageSize;

	FOnAssetSelected OnAssetSelected;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void LoadAssetsAndCreateButtons(TArray<FAsset> Assets);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void ClearAllButtons();
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void UpdateSelectedButton(TSubclassOf<URpmAssetButtonWidget> AssetButton);

	UFUNCTION()
	void OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButtonWidget);
	
	void CreateButton(const FAsset& AssetData);

private:
	TArray<TSubclassOf<URpmAssetButtonWidget>> AssetButtons;
};
