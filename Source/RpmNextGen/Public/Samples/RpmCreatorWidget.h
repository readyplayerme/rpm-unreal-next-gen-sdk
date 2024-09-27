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
	
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void CreateAssetPanelsFromCategories(const TArray<FString>& CategoryArray);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void SwitchToPanel(const FString& Category);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* AssetPanelSwitcher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TSubclassOf<URpmAssetPanelWidget> AssetPanelBlueprint;

	virtual void SynchronizeProperties() override;

	UPROPERTY(BlueprintAssignable, Category = "Events" )
	FOnAssetSelected OnAssetSelected;

	UFUNCTION()
	void HandleAssetSelectedFromPanel(const FAsset& AssetData);
	
	TMap<FString, int32> IndexMapByCategory;
private:
	UUserWidget* CreateAssetPanel(const FString& Category);
};
