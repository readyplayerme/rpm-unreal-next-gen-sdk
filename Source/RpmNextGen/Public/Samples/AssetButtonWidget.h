// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "AssetButtonWidget.generated.h"

class UImage;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetButtonClicked, const FAsset&, AssetData);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API UAssetButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAssetButtonClicked OnAssetButtonClicked;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeButton(const FAsset& InAssetData, const FVector2D& InImageSize);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* AssetButton;

	UPROPERTY(meta = (BindWidget))
	UImage* AssetImage;

	FAsset AssetData;

	UFUNCTION()
	void HandleButtonClicked();

	void SetImageFromURL(UImage* Image, const FString& URL);
	void OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TWeakObjectPtr<UImage> Image);
};
