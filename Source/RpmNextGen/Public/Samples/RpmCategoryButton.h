// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RpmCategoryButton.generated.h"

class UImage;
class UBorder;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryClicked, const FString&, CategoryName);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmCategoryButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UBorder* SelectionBorder;
	
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
	void Initialize(FString Category, UTexture2D* Image);

	UFUNCTION(BlueprintCallable, Category = "Category Button")
	void SetSelected(bool bInIsSelected);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Button" )
	FString AssetCategoryName;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void SynchronizeProperties() override;
	
private:
	UFUNCTION()
	void HandleButtonClicked();

	bool bIsSelected;
};
