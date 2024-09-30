// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RpmCategoryButtonWidget.generated.h"

class UImage;
class UBorder;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryClicked, URpmCategoryButtonWidget*, CategoryButton);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta = (BindWidget))
	UImage* CategoryImage;

	UPROPERTY(meta = (BindWidget))
	UButton* CategoryButton;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCategoryClicked OnCategoryClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Button" )
	FLinearColor SelectedColor = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Button")
	UTexture2D* CategoryImageTexture;
	
	UFUNCTION(BlueprintCallable, Category = "Category Button")
	virtual void InitializeButton(FString Category, UTexture2D* Image, const FVector2D& InImageSize);

	UFUNCTION(BlueprintCallable, Category = "Category Button")
	virtual void SetSelected(bool bIsSelected);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Button" )
	FString AssetCategoryName;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	
private:
	FLinearColor DefaultColor;
	
	UFUNCTION()
	virtual void HandleButtonClicked();
};
