// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|UI")
	void CreateAssetPanelsFromCategories(const TArray<FString>& CategoryNames);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|UI")
	void SwitchToPanel(const FString& WidgetName);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* AssetPanelSwitcher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me|UI")
	TSubclassOf<UUserWidget> WidgetBlueprintClass;

	virtual void SynchronizeProperties() override;
	
	TMap<FString, int32> IndexMapByCategory;
private:
	UUserWidget* CreateWidgetFromName(const FString& WidgetName);
	
};
