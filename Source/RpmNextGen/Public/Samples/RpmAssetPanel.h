// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetPanel.generated.h"

class FAssetApi;
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
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel" )
	TSubclassOf<URpmAssetButtonWidget> AssetButtonBlueprint;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ButtonContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel")
	URpmAssetButtonWidget* SelectedAssetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	FVector2D ImageSize;

	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnAssetSelected OnAssetSelected;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void CreateButtonsFromAssets(TArray<FAsset> Assets);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void ClearAllButtons();
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void UpdateSelectedButton(URpmAssetButtonWidget* AssetButton);

	UFUNCTION()
	void OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButtonWidget);
	
	UFUNCTION()
	void OnAssetListResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Panel")
	void LoadAssetsOfType(const FString& AssetType);
	
	void CreateButton(const FAsset& AssetData);
private:
	TArray<TSubclassOf<URpmAssetButtonWidget>> AssetButtons;
	TSharedPtr<FAssetApi> AssetApi;
};
