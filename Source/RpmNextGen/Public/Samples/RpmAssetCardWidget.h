// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetCardWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class RPMNEXTGEN_API URpmAssetCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Asset Card")
	virtual void InitializeCard(const FAsset& Asset);

	UFUNCTION(BlueprintCallable, Category = "Asset Card")
	void LoadImage(const FString& URL);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AssetCategoryText;
	
	UPROPERTY(meta = (BindWidget))
	UImage* AssetImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AssetNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AssetIdText;
	
private:
	FAsset AssetData;
};
