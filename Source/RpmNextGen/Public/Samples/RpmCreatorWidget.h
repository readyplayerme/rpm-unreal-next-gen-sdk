// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RpmAssetPanelWidget.h"
#include "Blueprint/UserWidget.h"
#include "RpmCreatorWidget.generated.h"

class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmCreatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void CreateAssetPanelsFromCategories(const TArray<FString>& CategoryArray);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void SwitchToPanel(const FString& Category);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me" )
	int32 PaginationLimit = 50;
	
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* AssetPanelSwitcher;
	
	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnAssetSelected OnAssetSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TSubclassOf<URpmAssetPanelWidget> AssetPanelBlueprint;
	TMap<FString, int32> IndexMapByCategory;
	
	UFUNCTION()
	void HandleAssetSelectedFromPanel(const FAsset& AssetData);
	
	virtual void SynchronizeProperties() override;

private:
	UUserWidget* CreateAssetPanel(const FString& Category);
};
