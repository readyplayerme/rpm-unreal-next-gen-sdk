// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetPanelWidget.generated.h"

class FAssetApi;
struct FAsset;
class URpmAssetButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetSelected, const FAsset&, AssetData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPaginationUpdated, const FPagination&, Pagination);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmAssetPanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ButtonContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel" )
	TSubclassOf<URpmAssetButtonWidget> AssetButtonBlueprint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Panel")
	URpmAssetButtonWidget* SelectedAssetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	FVector2D ButtonSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	FVector2D ImageSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	int32 PaginationLimit = 50;

	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnAssetSelected OnAssetSelected;
	
	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FPaginationUpdated OnPaginationUpdated;
	
	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	void CreateButtonsFromAssets(TArray<FAsset> Assets);

	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	void ClearAllButtons();
	
	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	void UpdateSelectedButton(URpmAssetButtonWidget* AssetButton);

	UFUNCTION()
	void OnAssetButtonClicked(const URpmAssetButtonWidget* AssetButtonWidget);

	void OnAssetListResponse(TSharedPtr<FAssetListResponse> Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	void LoadAssetsFromCache(const FString& AssetType);
	
	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	void LoadAssetsOfType(const FString& AssetType);
	
	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	virtual void LoadNextPage();
	UFUNCTION(BlueprintCallable, Category = "Asset Panel")
	virtual void LoadPreviousPage();

	void CreateButton(const FAsset& AssetData);

	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	
private:
	FPagination Pagination;
	FString CurrentAssetType;
	TMap<FString, TSubclassOf<URpmAssetButtonWidget>> AssetButtonMap;
	TSharedPtr<FAssetApi> AssetApi;
};
