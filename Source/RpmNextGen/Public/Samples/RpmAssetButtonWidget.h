// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "RpmAssetButtonWidget.generated.h"

class UBorder;
class UImage;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetButtonClicked, const FAsset&, AssetData);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmAssetButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAssetButtonClicked OnAssetButtonClicked;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeButton(const FAsset& InAssetData, const FVector2D& InImageSize);

	UFUNCTION(BlueprintCallable, Category = "Category Button")
	void SetSelected(bool bIsSelected);

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Button" )
	FLinearColor SelectedColor = FLinearColor::Yellow;

private:

	UPROPERTY(meta = (BindWidget))
	UBorder* SelectionBorder;
	
	UPROPERTY(meta = (BindWidget))
	UButton* AssetButton;

	UPROPERTY(meta = (BindWidget))
	UImage* AssetImage;

	FAsset AssetData;

	UFUNCTION()
	void HandleButtonClicked();
};
