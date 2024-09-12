// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetButtonWidget.generated.h"

class USizeBox;
class UBorder;
class UImage;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetButtonClicked, const URpmAssetButtonWidget*, AssetData);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmAssetButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	UButton* AssetButton;

	UPROPERTY(meta = (BindWidget))
	UImage* AssetImage;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAssetButtonClicked OnAssetButtonClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	FLinearColor SelectedColor = FLinearColor::Yellow;

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Button")
	virtual void InitializeButton(const FAsset& InAssetData, const FVector2D& InImageSize);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me|Asset Button")
	virtual void SetSelected(const bool bInIsSelected);

	FAsset GetAssetData() const { return AssetData; }
protected:
	virtual void NativeConstruct() override;

private:
	FLinearColor DefaultColor;

	FAsset AssetData;
	UFUNCTION()
	virtual void HandleButtonClicked();

	bool bIsSelected;
};
