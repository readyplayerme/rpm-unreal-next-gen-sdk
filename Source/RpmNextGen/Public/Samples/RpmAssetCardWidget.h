// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetCardWidget.generated.h"

class FRpmTextureLoader;
class UImage;
class UTextBlock;

UCLASS()
class RPMNEXTGEN_API URpmAssetCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AssetCategoryText;
	
	UPROPERTY(meta = (BindWidget))
	UImage* AssetImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AssetNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AssetIdText;
	
	UFUNCTION()
	void OnTextureLoaded(UTexture2D* Texture2D);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void InitializeCard(const FAsset& Asset);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadImage(const FAsset& Asset);

	virtual void NativeConstruct() override;
	
private:
	FAsset AssetData;
	TSharedPtr<FRpmTextureLoader> TextureLoader;
};
